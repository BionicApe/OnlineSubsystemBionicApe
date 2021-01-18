// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

/**
 * Online subsystem module class  (BionicApe Implementation)
 * Code related to the loading of the BionicApe module
 */
class FOnlineSubsystemBionicApeModule : public IModuleInterface
{
private:

	/** Class responsible for creating instance(s) of the subsystem */
	class FOnlineFactoryBionicApe* BionicApeFactory;

public:

	FOnlineSubsystemBionicApeModule() : 
		BionicApeFactory(NULL)
	{}

	virtual ~FOnlineSubsystemBionicApeModule() {}

	// IModuleInterface

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

	virtual bool SupportsAutomaticShutdown() override
	{
		return false;
	}
};
