// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineAsyncTaskManager.h"

/**
 *	BionicApe version of the async task manager to register the various BionicApe callbacks with the engine
 */
class FOnlineAsyncTaskManagerBionicApe : public FOnlineAsyncTaskManager
{
protected:

	/** Cached reference to the main online subsystem */
	class FOnlineSubsystemBionicApe* BionicApeSubsystem;

public:

	FOnlineAsyncTaskManagerBionicApe(class FOnlineSubsystemBionicApe* InOnlineSubsystem)
		: BionicApeSubsystem(InOnlineSubsystem)
	{
	}

	~FOnlineAsyncTaskManagerBionicApe() 
	{
	}

	// FOnlineAsyncTaskManager
	virtual void OnlineTick() override;
};
