// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemBionicApeTypes.h"
#include "NboSerializer.h"

/**
 * Serializes data in network byte order form into a buffer
 */
class FNboSerializeToBufferBionicApe : public FNboSerializeToBuffer
{
public:
	/** Default constructor zeros num bytes*/
	FNboSerializeToBufferBionicApe() :
		FNboSerializeToBuffer(512)
	{
	}

	/** Constructor specifying the size to use */
	FNboSerializeToBufferBionicApe(uint32 Size) :
		FNboSerializeToBuffer(Size)
	{
	}

	/**
	 * Adds BionicApe session info to the buffer
	 */
 	friend inline FNboSerializeToBufferBionicApe& operator<<(FNboSerializeToBufferBionicApe& Ar, const FOnlineSessionInfoBionicApe& SessionInfo)
 	{
		check(SessionInfo.HostAddr.IsValid());
		// Skip SessionType (assigned at creation)
		Ar << SessionInfo.SessionId;
		Ar << *SessionInfo.HostAddr;
		return Ar;
 	}

	/**
	 * Adds BionicApe Unique Id to the buffer
	 */
	friend inline FNboSerializeToBufferBionicApe& operator<<(FNboSerializeToBufferBionicApe& Ar, const FUniqueNetIdBionicApe& UniqueId)
	{
		Ar << UniqueId.UniqueNetIdStr;
		return Ar;
	}
};

/**
 * Class used to write data into packets for sending via system link
 */
class FNboSerializeFromBufferBionicApe : public FNboSerializeFromBuffer
{
public:
	/**
	 * Initializes the buffer, size, and zeros the read offset
	 */
	FNboSerializeFromBufferBionicApe(uint8* Packet,int32 Length) :
		FNboSerializeFromBuffer(Packet,Length)
	{
	}

	/**
	 * Reads BionicApe session info from the buffer
	 */
 	friend inline FNboSerializeFromBufferBionicApe& operator>>(FNboSerializeFromBufferBionicApe& Ar, FOnlineSessionInfoBionicApe& SessionInfo)
 	{
		check(SessionInfo.HostAddr.IsValid());
		// Skip SessionType (assigned at creation)
		Ar >> SessionInfo.SessionId; 
		Ar >> *SessionInfo.HostAddr;
		return Ar;
 	}

	/**
	 * Reads BionicApe Unique Id from the buffer
	 */
	friend inline FNboSerializeFromBufferBionicApe& operator>>(FNboSerializeFromBufferBionicApe& Ar, FUniqueNetIdBionicApe& UniqueId)
	{
		Ar >> UniqueId.UniqueNetIdStr;
		return Ar;
	}
};
