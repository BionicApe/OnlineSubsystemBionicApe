// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemImpl.h"
#include "OnlineSubsystemBionicApePackage.h"
#include "HAL/ThreadSafeCounter.h"

class FOnlineAchievementsBionicApe;
class FOnlineIdentityBionicApe;
class FOnlineLeaderboardsBionicApe;
class FOnlineSessionBionicApe;
class FOnlineVoiceImpl;

/** Forward declarations of all interface classes */
typedef TSharedPtr<class FOnlineSessionBionicApe, ESPMode::ThreadSafe> FOnlineSessionBionicApePtr;
typedef TSharedPtr<class FOnlineProfileBionicApe, ESPMode::ThreadSafe> FOnlineProfileBionicApePtr;
typedef TSharedPtr<class FOnlineFriendsBionicApe, ESPMode::ThreadSafe> FOnlineFriendsBionicApePtr;
typedef TSharedPtr<class FOnlineUserCloudBionicApe, ESPMode::ThreadSafe> FOnlineUserCloudBionicApePtr;
typedef TSharedPtr<class FOnlineLeaderboardsBionicApe, ESPMode::ThreadSafe> FOnlineLeaderboardsBionicApePtr;
typedef TSharedPtr<class FOnlineVoiceImpl, ESPMode::ThreadSafe> FOnlineVoiceImplPtr;
typedef TSharedPtr<class FOnlineExternalUIBionicApe, ESPMode::ThreadSafe> FOnlineExternalUIBionicApePtr;
typedef TSharedPtr<class FOnlineIdentityBionicApe, ESPMode::ThreadSafe> FOnlineIdentityBionicApePtr;
typedef TSharedPtr<class FOnlineAchievementsBionicApe, ESPMode::ThreadSafe> FOnlineAchievementsBionicApePtr;
typedef TSharedPtr<class FOnlineStoreV2BionicApe, ESPMode::ThreadSafe> FOnlineStoreV2BionicApePtr;
typedef TSharedPtr<class FOnlinePurchaseBionicApe, ESPMode::ThreadSafe> FOnlinePurchaseBionicApePtr;

/**
 *	OnlineSubsystemBionicApe - Implementation of the online subsystem for BionicApe services
 */
class ONLINESUBSYSTEMBIONICAPE_API FOnlineSubsystemBionicApe : 
	public FOnlineSubsystemImpl
{

public:

	virtual ~FOnlineSubsystemBionicApe() = default;

	// IOnlineSubsystem

	virtual IOnlineSessionPtr GetSessionInterface() const override;
	virtual IOnlineFriendsPtr GetFriendsInterface() const override;
	virtual IOnlinePartyPtr GetPartyInterface() const override;
	virtual IOnlineGroupsPtr GetGroupsInterface() const override;
	virtual IOnlineSharedCloudPtr GetSharedCloudInterface() const override;
	virtual IOnlineUserCloudPtr GetUserCloudInterface() const override;
	virtual IOnlineEntitlementsPtr GetEntitlementsInterface() const override;
	virtual IOnlineLeaderboardsPtr GetLeaderboardsInterface() const override;
	virtual IOnlineVoicePtr GetVoiceInterface() const override;
	virtual IOnlineExternalUIPtr GetExternalUIInterface() const override;	
	virtual IOnlineTimePtr GetTimeInterface() const override;
	virtual IOnlineIdentityPtr GetIdentityInterface() const override;
	virtual IOnlineTitleFilePtr GetTitleFileInterface() const override;
	virtual IOnlineStoreV2Ptr GetStoreV2Interface() const override;
	virtual IOnlinePurchasePtr GetPurchaseInterface() const override;
	virtual IOnlineEventsPtr GetEventsInterface() const override;
	virtual IOnlineAchievementsPtr GetAchievementsInterface() const override;
	virtual IOnlineSharingPtr GetSharingInterface() const override;
	virtual IOnlineUserPtr GetUserInterface() const override;
	virtual IOnlineMessagePtr GetMessageInterface() const override;
	virtual IOnlinePresencePtr GetPresenceInterface() const override;
	virtual IOnlineChatPtr GetChatInterface() const override;
	virtual IOnlineStatsPtr GetStatsInterface() const override;
	virtual IOnlineTurnBasedPtr GetTurnBasedInterface() const override;
	virtual IOnlineTournamentPtr GetTournamentInterface() const override;

	virtual bool Init() override;
	virtual bool Shutdown() override;
	virtual FString GetAppId() const override;
	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	virtual FText GetOnlineServiceName() const override;

	// FTickerObjectBase
	
	virtual bool Tick(float DeltaTime) override;

	// FOnlineSubsystemBionicApe

PACKAGE_SCOPE:

	/** Only the factory makes instances */
	FOnlineSubsystemBionicApe() = delete;
	explicit FOnlineSubsystemBionicApe(FName InInstanceName) :
		FOnlineSubsystemImpl(TEXT("BionicApe"), InInstanceName),
		SessionInterface(nullptr),
		VoiceInterface(nullptr),
		bVoiceInterfaceInitialized(false),
		LeaderboardsInterface(nullptr),
		IdentityInterface(nullptr),
		AchievementsInterface(nullptr),
		StoreV2Interface(nullptr),
		OnlineAsyncTaskThreadRunnable(nullptr),
		OnlineAsyncTaskThread(nullptr)
	{}

private:

	/** Interface to the session services */
	FOnlineSessionBionicApePtr SessionInterface;

	/** Interface for voice communication */
	mutable IOnlineVoicePtr VoiceInterface;

	/** Interface for voice communication */
	mutable bool bVoiceInterfaceInitialized;

	/** Interface to the leaderboard services */
	FOnlineLeaderboardsBionicApePtr LeaderboardsInterface;

	/** Interface to the identity registration/auth services */
	FOnlineIdentityBionicApePtr IdentityInterface;

	/** Interface for achievements */
	FOnlineAchievementsBionicApePtr AchievementsInterface;

	/** Interface for store */
	FOnlineStoreV2BionicApePtr StoreV2Interface;

	/** Interface for purchases */
	FOnlinePurchaseBionicApePtr PurchaseInterface;

	/** Online async task runnable */
	class FOnlineAsyncTaskManagerBionicApe* OnlineAsyncTaskThreadRunnable;

	/** Online async task thread */
	class FRunnableThread* OnlineAsyncTaskThread;

	// task counter, used to generate unique thread names for each task
	static FThreadSafeCounter TaskCounter;
};

typedef TSharedPtr<FOnlineSubsystemBionicApe, ESPMode::ThreadSafe> FOnlineSubsystemBionicApePtr;

