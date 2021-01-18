// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnlineIdentityBionicApe.h"
#include "Misc/CommandLine.h"
#include "Misc/Guid.h"
#include "Misc/OutputDeviceRedirector.h"
#include "OnlineSubsystemBionicApe.h"
#include "IPAddress.h"
#include "SocketSubsystem.h"
#include "OnlineError.h"
#include "BAUsers.h"
#include "BAUser.h"
#include "Misc/DefaultValueHelper.h"

FOnlineIdentityBionicApe::FOnlineIdentityBionicApe(FOnlineSubsystemBionicApe* InSubsystem) : BionicApeSubsystem(InSubsystem)
{
	//FSoftObjectPath Target = FSoftObjectPath("/Game/ThePrison/Model/Users.Users");
	//TSoftObjectPtr<UBAUsers> BAUsers = TSoftObjectPtr<UBAUsers>(FSoftObjectPath(Target));

	//if (BAUsers.IsValid())
	//{
	//	BAUsers.LoadSynchronous();

	//	UsersById = BAUsers->Users;	
	//}
}

FOnlineIdentityBionicApe::~FOnlineIdentityBionicApe()
{
}

bool FUserOnlineAccountBionicApe::GetAuthAttribute(const FString& AttrName, FString& OutAttrValue) const
{
	const FString* FoundAttr = AdditionalAuthData.Find(AttrName);
	if (FoundAttr != NULL)
	{
		OutAttrValue = *FoundAttr;
		return true;
	}
	return false;
}

bool FUserOnlineAccountBionicApe::GetUserAttribute(const FString& AttrName, FString& OutAttrValue) const
{
	const FString* FoundAttr = UserAttributes.Find(AttrName);
	if (FoundAttr != NULL)
	{
		OutAttrValue = *FoundAttr;
		return true;
	}
	return false;
}

bool FUserOnlineAccountBionicApe::SetUserAttribute(const FString& AttrName, const FString& AttrValue)
{
	const FString* FoundAttr = UserAttributes.Find(AttrName);
	if (FoundAttr == NULL || *FoundAttr != AttrValue)
	{
		UserAttributes.Add(AttrName, AttrValue);
		return true;
	}
	return false;
}

bool FOnlineIdentityBionicApe::Login(int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials)
{
	FString ErrorStr;
	// valid local player index
	if (LocalUserNum < 0 || LocalUserNum >= MAX_LOCAL_PLAYERS)
	{
		ErrorStr = FString::Printf(TEXT("Invalid LocalUserNum=%d"), LocalUserNum);
		UE_LOG_ONLINE_IDENTITY(Warning, TEXT("Login request failed. %s"), *ErrorStr);
		TriggerOnLoginCompleteDelegates(LocalUserNum, false, FUniqueNetIdBionicApe(), ErrorStr);
		return false;
	}

	if (ValidateCredentials(AccountCredentials))
	{
		TSharedPtr<FUserOnlineAccountBionicApe> UserAccountPtr;
		//FString UserIdStr = FString::FromInt(BAUser->UserID);

		//FUniqueNetIdBionicApe NewUserId(UserIdStr);
		FUniqueNetIdBionicApe NewUserId(AccountCredentials.Id);

		//UserAccountPtr = MakeShareable(new FUserOnlineAccountBionicApe(UserIdStr));
		//UserAccountPtr->UserAttributes.Add(USER_ATTR_ID, UserIdStr);
		UserAccountPtr = MakeShareable(new FUserOnlineAccountBionicApe(AccountCredentials.Id));
		UserAccountPtr->UserAttributes.Add(USER_ATTR_ID, AccountCredentials.Id);

		// update/add cached entry for user
		UserAccounts.Add(NewUserId, UserAccountPtr.ToSharedRef());

		// keep track of user ids for local users
		UserIds.Add(LocalUserNum, UserAccountPtr->GetUserId());

		TriggerOnLoginCompleteDelegates(LocalUserNum, true, *UserAccountPtr->GetUserId(), ErrorStr);
		return true;
	}

	ErrorStr = FString::Printf(TEXT("Invalid Account Credentials  id=%s"), *AccountCredentials.Id);
	UE_LOG_ONLINE_IDENTITY(Warning, TEXT("Login request failed for User. %s"), *ErrorStr);
	TriggerOnLoginCompleteDelegates(LocalUserNum, false, FUniqueNetIdBionicApe(), *ErrorStr);

	return false;
}

bool FOnlineIdentityBionicApe::Logout(int32 LocalUserNum)
{
	TSharedPtr<const FUniqueNetId> UserId = GetUniquePlayerId(LocalUserNum);
	if (UserId.IsValid())
	{
		// remove cached user account
		UserAccounts.Remove(FUniqueNetIdBionicApe(*UserId));
		// remove cached user id
		UserIds.Remove(LocalUserNum);
		// not async but should call completion delegate anyway
		TriggerOnLogoutCompleteDelegates(LocalUserNum, true);

		return true;
	}
	else
	{
		UE_LOG_ONLINE_IDENTITY(Warning, TEXT("No logged in user found for LocalUserNum=%d."),
			LocalUserNum);
		TriggerOnLogoutCompleteDelegates(LocalUserNum, false);
	}
	return false;
}

bool FOnlineIdentityBionicApe::AutoLogin(int32 LocalUserNum)
{
	FString LoginStr;
	FString PasswordStr;
	FString TypeStr;

	FParse::Value(FCommandLine::Get(), TEXT("AUTH_LOGIN="), LoginStr);
	FParse::Value(FCommandLine::Get(), TEXT("AUTH_PASSWORD="), PasswordStr);
	FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), TypeStr);

	bool bEnableWarning = LoginStr.Len() > 0 || PasswordStr.Len() > 0 || TypeStr.Len() > 0;

	if (!LoginStr.IsEmpty())
	{
		if (!PasswordStr.IsEmpty())
		{
			if (!TypeStr.IsEmpty())
			{
				return Login(0, FOnlineAccountCredentials(TypeStr, LoginStr, PasswordStr));
			}
			else if (bEnableWarning)
			{
				UE_LOG_ONLINE_IDENTITY(Warning, TEXT("AutoLogin missing AUTH_TYPE=<type>."));
			}
		}
		else if (bEnableWarning)
		{
			UE_LOG_ONLINE_IDENTITY(Warning, TEXT("AutoLogin missing AUTH_PASSWORD=<password>."));
		}
	}
	else if (bEnableWarning)
	{
		UE_LOG_ONLINE_IDENTITY(Warning, TEXT("AutoLogin missing AUTH_LOGIN=<login id>."));
	}

	return false;
}

TSharedPtr<FUserOnlineAccount> FOnlineIdentityBionicApe::GetUserAccount(const FUniqueNetId& UserId) const
{
	TSharedPtr<FUserOnlineAccount> Result;

	FUniqueNetIdBionicApe StringUserId(UserId);
	const TSharedRef<FUserOnlineAccountBionicApe>* FoundUserAccount = UserAccounts.Find(StringUserId);
	if (FoundUserAccount != NULL)
	{
		Result = *FoundUserAccount;
	}

	return Result;
}

TArray<TSharedPtr<FUserOnlineAccount> > FOnlineIdentityBionicApe::GetAllUserAccounts() const
{
	TArray<TSharedPtr<FUserOnlineAccount>> Result;

	for (TMap<FUniqueNetIdBionicApe, TSharedRef<FUserOnlineAccountBionicApe>>::TConstIterator It(UserAccounts); It; ++It)
	{
		Result.Add(It.Value());
	}

	return Result;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityBionicApe::GetUniquePlayerId(int32 LocalUserNum) const
{
	const TSharedPtr<const FUniqueNetId>* FoundId = UserIds.Find(LocalUserNum);
	if (FoundId != NULL)
	{
		return *FoundId;
	}
	return NULL;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityBionicApe::CreateUniquePlayerId(uint8* Bytes, int32 Size)
{
	if (Bytes != NULL && Size > 0)
	{
		FString StrId(Size, (TCHAR*)Bytes);
		return MakeShareable(new FUniqueNetIdBionicApe(StrId));
	}
	return NULL;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityBionicApe::CreateUniquePlayerId(const FString& Str)
{
	return MakeShareable(new FUniqueNetIdBionicApe(Str));
}

ELoginStatus::Type FOnlineIdentityBionicApe::GetLoginStatus(int32 LocalUserNum) const
{
	TSharedPtr<const FUniqueNetId> UserId = GetUniquePlayerId(LocalUserNum);
	if (UserId.IsValid())
	{
		return GetLoginStatus(*UserId);
	}
	return ELoginStatus::NotLoggedIn;
}

ELoginStatus::Type FOnlineIdentityBionicApe::GetLoginStatus(const FUniqueNetId& UserId) const
{
	TSharedPtr<FUserOnlineAccount> UserAccount = GetUserAccount(UserId);
	if (UserAccount.IsValid() &&
		UserAccount->GetUserId()->IsValid())
	{
		return ELoginStatus::LoggedIn;
	}
	return ELoginStatus::NotLoggedIn;
}

FString FOnlineIdentityBionicApe::GetPlayerNickname(int32 LocalUserNum) const
{
	TSharedPtr<const FUniqueNetId> UniqueId = GetUniquePlayerId(LocalUserNum);
	if (UniqueId.IsValid())
	{
		return UniqueId->ToString();
	}

	return TEXT("BionicApeUser");
}

FString FOnlineIdentityBionicApe::GetPlayerNickname(const FUniqueNetId& UserId) const
{
	return UserId.ToString();
}

FString FOnlineIdentityBionicApe::GetAuthToken(int32 LocalUserNum) const
{
	TSharedPtr<const FUniqueNetId> UserId = GetUniquePlayerId(LocalUserNum);
	if (UserId.IsValid())
	{
		TSharedPtr<FUserOnlineAccount> UserAccount = GetUserAccount(*UserId);
		if (UserAccount.IsValid())
		{
			return UserAccount->GetAccessToken();
		}
	}
	return FString();
}

void FOnlineIdentityBionicApe::RevokeAuthToken(const FUniqueNetId& UserId, const FOnRevokeAuthTokenCompleteDelegate& Delegate)
{
	UE_LOG_ONLINE_IDENTITY(Display, TEXT("FOnlineIdentityBionicApe::RevokeAuthToken not implemented"));
	TSharedRef<const FUniqueNetId> UserIdRef(UserId.AsShared());
	BionicApeSubsystem->ExecuteNextTick([UserIdRef, Delegate]()
		{
			Delegate.ExecuteIfBound(*UserIdRef, FOnlineError(FString(TEXT("RevokeAuthToken not implemented"))));
		});
}


void FOnlineIdentityBionicApe::GetUserPrivilege(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate)
{
	Delegate.ExecuteIfBound(UserId, Privilege, (uint32)EPrivilegeResults::NoFailures);
}

FPlatformUserId FOnlineIdentityBionicApe::GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId) const
{
	for (int i = 0; i < MAX_LOCAL_PLAYERS; ++i)
	{
		auto CurrentUniqueId = GetUniquePlayerId(i);
		if (CurrentUniqueId.IsValid() && (*CurrentUniqueId == UniqueNetId))
		{
			return i;
		}
	}

	return PLATFORMUSERID_NONE;
}

FString FOnlineIdentityBionicApe::GetAuthType() const
{
	return TEXT("");
}


bool FOnlineIdentityBionicApe::ValidateCredentials(const FOnlineAccountCredentials& AccountCredentials)
{
	int32 Index;
	if (FDefaultValueHelper::ParseInt(AccountCredentials.Id, Index))
	{
		if (Passwords.Contains(Index))
		{
			return Passwords[Index] == AccountCredentials.Token;
		}
	}
	return false;
}

//UBAUser* FOnlineIdentityBionicApe::FindUserByUsername(const FString& Username) const
//{
//	for (auto It = UsersById.CreateConstIterator(); It; ++It)
//	{
//		if (UBAUser* ItUser = Cast<UBAUser>(It.Value()))
//		{
//			if (ItUser->Username.Equals(Username))
//			{
//				return ItUser;
//			}
//		}
//	}
//	return nullptr;
//}

//UBAUser* FOnlineIdentityBionicApe::GetBAUser(const FUniqueNetId& UniqueId) const
//{
//	FUniqueNetIdBionicApe StringUserId(UniqueId);
//	int32 UserId;
//	if (FDefaultValueHelper::ParseInt(StringUserId.UniqueNetIdStr, UserId) && UsersById.Contains(UserId))
//	{
//		return UsersById[UserId];		
//	}
//	return nullptr;
//}