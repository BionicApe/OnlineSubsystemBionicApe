// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnlineSubsystemBionicApe.h"
#include "HAL/RunnableThread.h"
#include "OnlineAsyncTaskManagerBionicApe.h"

#include "OnlineSessionInterfaceBionicApe.h"
#include "OnlineLeaderboardInterfaceBionicApe.h"
#include "OnlineIdentityBionicApe.h"
#include "VoiceInterfaceBionicApe.h"
#include "OnlineAchievementsInterfaceBionicApe.h"
#include "OnlineStoreV2InterfaceBionicApe.h"
#include "OnlinePurchaseInterfaceBionicApe.h"

FThreadSafeCounter FOnlineSubsystemBionicApe::TaskCounter;

IOnlineSessionPtr FOnlineSubsystemBionicApe::GetSessionInterface() const
{
	return SessionInterface;
}

IOnlineFriendsPtr FOnlineSubsystemBionicApe::GetFriendsInterface() const
{
	return nullptr;
}

IOnlinePartyPtr FOnlineSubsystemBionicApe::GetPartyInterface() const
{
	return nullptr;
}

IOnlineGroupsPtr FOnlineSubsystemBionicApe::GetGroupsInterface() const
{
	return nullptr;
}

IOnlineSharedCloudPtr FOnlineSubsystemBionicApe::GetSharedCloudInterface() const
{
	return nullptr;
}

IOnlineUserCloudPtr FOnlineSubsystemBionicApe::GetUserCloudInterface() const
{
	return nullptr;
}

IOnlineEntitlementsPtr FOnlineSubsystemBionicApe::GetEntitlementsInterface() const
{
	return nullptr;
};

IOnlineLeaderboardsPtr FOnlineSubsystemBionicApe::GetLeaderboardsInterface() const
{
	return LeaderboardsInterface;
}

IOnlineVoicePtr FOnlineSubsystemBionicApe::GetVoiceInterface() const
{
	if (VoiceInterface.IsValid() && !bVoiceInterfaceInitialized)
	{	
		if (!VoiceInterface->Init())
		{
			VoiceInterface = nullptr;
		}

		bVoiceInterfaceInitialized = true;
	}

	return VoiceInterface;
}

IOnlineExternalUIPtr FOnlineSubsystemBionicApe::GetExternalUIInterface() const
{
	return nullptr;
}

IOnlineTimePtr FOnlineSubsystemBionicApe::GetTimeInterface() const
{
	return nullptr;
}

IOnlineIdentityPtr FOnlineSubsystemBionicApe::GetIdentityInterface() const
{
	return IdentityInterface;
}

IOnlineTitleFilePtr FOnlineSubsystemBionicApe::GetTitleFileInterface() const
{
	return nullptr;
}

IOnlineStoreV2Ptr FOnlineSubsystemBionicApe::GetStoreV2Interface() const
{
	return StoreV2Interface;
}

IOnlinePurchasePtr FOnlineSubsystemBionicApe::GetPurchaseInterface() const
{
	return PurchaseInterface;
}

IOnlineEventsPtr FOnlineSubsystemBionicApe::GetEventsInterface() const
{
	return nullptr;
}

IOnlineAchievementsPtr FOnlineSubsystemBionicApe::GetAchievementsInterface() const
{
	return AchievementsInterface;
}

IOnlineSharingPtr FOnlineSubsystemBionicApe::GetSharingInterface() const
{
	return nullptr;
}

IOnlineUserPtr FOnlineSubsystemBionicApe::GetUserInterface() const
{
	return nullptr;
}

IOnlineMessagePtr FOnlineSubsystemBionicApe::GetMessageInterface() const
{
	return nullptr;
}

IOnlinePresencePtr FOnlineSubsystemBionicApe::GetPresenceInterface() const
{
	return nullptr;
}

IOnlineChatPtr FOnlineSubsystemBionicApe::GetChatInterface() const
{
	return nullptr;
}

IOnlineStatsPtr FOnlineSubsystemBionicApe::GetStatsInterface() const
{
	return nullptr;
}

IOnlineTurnBasedPtr FOnlineSubsystemBionicApe::GetTurnBasedInterface() const
{
	return nullptr;
}

IOnlineTournamentPtr FOnlineSubsystemBionicApe::GetTournamentInterface() const
{
	return nullptr;
}

bool FOnlineSubsystemBionicApe::Tick(float DeltaTime)
{
	if (!FOnlineSubsystemImpl::Tick(DeltaTime))
	{
		return false;
	}

	if (OnlineAsyncTaskThreadRunnable)
	{
		OnlineAsyncTaskThreadRunnable->GameTick();
	}

	if (SessionInterface.IsValid())
	{
		SessionInterface->Tick(DeltaTime);
	}

	if (VoiceInterface.IsValid() && bVoiceInterfaceInitialized)
	{
		VoiceInterface->Tick(DeltaTime);
	}

	return true;
}

bool FOnlineSubsystemBionicApe::Init()
{
	const bool bBionicApeInit = true;
	
	if (bBionicApeInit)
	{
		// Create the online async task thread
		OnlineAsyncTaskThreadRunnable = new FOnlineAsyncTaskManagerBionicApe(this);
		check(OnlineAsyncTaskThreadRunnable);
		OnlineAsyncTaskThread = FRunnableThread::Create(OnlineAsyncTaskThreadRunnable, *FString::Printf(TEXT("OnlineAsyncTaskThreadBionicApe %s(%d)"), *InstanceName.ToString(), TaskCounter.Increment()), 128 * 1024, TPri_Normal);
		check(OnlineAsyncTaskThread);
		UE_LOG_ONLINE(Verbose, TEXT("Created thread (ID:%d)."), OnlineAsyncTaskThread->GetThreadID());

		SessionInterface = MakeShareable(new FOnlineSessionBionicApe(this));
		LeaderboardsInterface = MakeShareable(new FOnlineLeaderboardsBionicApe(this));
		IdentityInterface = MakeShareable(new FOnlineIdentityBionicApe(this));
		AchievementsInterface = MakeShareable(new FOnlineAchievementsBionicApe(this));
		VoiceInterface = MakeShareable(new FOnlineVoiceImpl(this));
		StoreV2Interface = MakeShareable(new FOnlineStoreV2BionicApe(*this));
		PurchaseInterface = MakeShareable(new FOnlinePurchaseBionicApe(*this));
	}
	else
	{
		Shutdown();
	}

	return bBionicApeInit;
}

bool FOnlineSubsystemBionicApe::Shutdown()
{
	UE_LOG_ONLINE(VeryVerbose, TEXT("FOnlineSubsystemBionicApe::Shutdown()"));

	FOnlineSubsystemImpl::Shutdown();

	if (OnlineAsyncTaskThread)
	{
		// Destroy the online async task thread
		delete OnlineAsyncTaskThread;
		OnlineAsyncTaskThread = nullptr;
	}

	if (OnlineAsyncTaskThreadRunnable)
	{
		delete OnlineAsyncTaskThreadRunnable;
		OnlineAsyncTaskThreadRunnable = nullptr;
	}

	if (VoiceInterface.IsValid() && bVoiceInterfaceInitialized)
	{
		VoiceInterface->Shutdown();
	}
	
#define DESTRUCT_INTERFACE(Interface) \
	if (Interface.IsValid()) \
	{ \
		ensure(Interface.IsUnique()); \
		Interface = nullptr; \
	}
 
	// Destruct the interfaces
	DESTRUCT_INTERFACE(PurchaseInterface);
	DESTRUCT_INTERFACE(StoreV2Interface);
	DESTRUCT_INTERFACE(VoiceInterface);
	DESTRUCT_INTERFACE(AchievementsInterface);
	DESTRUCT_INTERFACE(IdentityInterface);
	DESTRUCT_INTERFACE(LeaderboardsInterface);
	DESTRUCT_INTERFACE(SessionInterface);
	
#undef DESTRUCT_INTERFACE
	
	return true;
}

FString FOnlineSubsystemBionicApe::GetAppId() const
{
	return TEXT("");
}

bool FOnlineSubsystemBionicApe::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (FOnlineSubsystemImpl::Exec(InWorld, Cmd, Ar))
	{
		return true;
	}
	return false;
}
FText FOnlineSubsystemBionicApe::GetOnlineServiceName() const
{
	return NSLOCTEXT("OnlineSubsystemBionicApe", "OnlineServiceName", "BionicApe");
}

