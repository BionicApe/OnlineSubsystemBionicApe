// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnlineSubsystemBionicApeModule.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "OnlineSubsystemBionicApeModule.h"
#include "OnlineSubsystemModule.h"
#include "OnlineSubsystemNames.h"
#include "OnlineSubsystem.h"

IMPLEMENT_MODULE(FOnlineSubsystemBionicApeModule, OnlineSubsystemBionicApe);

/**
 * Class responsible for creating instance(s) of the subsystem
 */
class FOnlineFactoryBionicApe : public IOnlineFactory
{
public:

	FOnlineFactoryBionicApe() {}
	virtual ~FOnlineFactoryBionicApe() {}

	virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName)
	{
		FOnlineSubsystemBionicApePtr OnlineSub = MakeShared<FOnlineSubsystemBionicApe, ESPMode::ThreadSafe>(InstanceName);
		if (OnlineSub->IsEnabled())
		{
			if(!OnlineSub->Init())
			{
				UE_LOG_ONLINE(Warning, TEXT("BionicApe API failed to initialize!"));
				OnlineSub->Shutdown();
				OnlineSub = NULL;
			}
		}
		else
		{
			UE_LOG_ONLINE(Warning, TEXT("BionicApe API disabled!"));
			OnlineSub->Shutdown();
			OnlineSub = NULL;
		}

		return OnlineSub;
	}
};

void FOnlineSubsystemBionicApeModule::StartupModule()
{
	BionicApeFactory = new FOnlineFactoryBionicApe();

	// Create and register our singleton factory with the main online subsystem for easy access
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.RegisterPlatformService(TEXT("BionicApe"), BionicApeFactory);
}

void FOnlineSubsystemBionicApeModule::ShutdownModule()
{
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.UnregisterPlatformService(TEXT("BionicApe"));
	
	delete BionicApeFactory;
	BionicApeFactory = NULL;
}
