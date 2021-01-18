// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnlineAsyncTaskManagerBionicApe.h"

void FOnlineAsyncTaskManagerBionicApe::OnlineTick()
{
	check(BionicApeSubsystem);
	check(FPlatformTLS::GetCurrentThreadId() == OnlineThreadId || !FPlatformProcess::SupportsMultithreading());
}

