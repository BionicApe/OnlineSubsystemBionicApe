// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineStoreInterfaceV2.h"
#include "OnlineSubsystemBionicApePackage.h"

class FOnlineSubsystemBionicApe;
class FUniqueNetIdBionicApe;

/**
 * Implementation for online store via BionicApe interface
 */
class FOnlineStoreV2BionicApe : public IOnlineStoreV2, public TSharedFromThis<FOnlineStoreV2BionicApe, ESPMode::ThreadSafe>
{
public:
	FOnlineStoreV2BionicApe(FOnlineSubsystemBionicApe& InBionicApeSubsystem);
	virtual ~FOnlineStoreV2BionicApe() = default;

public:// IOnlineStoreV2
	virtual void QueryCategories(const FUniqueNetId& UserId, const FOnQueryOnlineStoreCategoriesComplete& Delegate) override;
	virtual void GetCategories(TArray<FOnlineStoreCategory>& OutCategories) const override;
	virtual void QueryOffersByFilter(const FUniqueNetId& UserId, const FOnlineStoreFilter& Filter, const FOnQueryOnlineStoreOffersComplete& Delegate) override;
	virtual void QueryOffersById(const FUniqueNetId& UserId, const TArray<FUniqueOfferId>& OfferIds, const FOnQueryOnlineStoreOffersComplete& Delegate) override;
	virtual void GetOffers(TArray<FOnlineStoreOfferRef>& OutOffers) const override;
	virtual TSharedPtr<FOnlineStoreOffer> GetOffer(const FUniqueOfferId& OfferId) const override;

PACKAGE_SCOPE:
	void QueryOffers(const FUniqueNetIdBionicApe& UserId, const TArray<FUniqueOfferId>& OfferIds, const FOnQueryOnlineStoreOffersComplete& Delegate);

PACKAGE_SCOPE:
	FOnlineSubsystemBionicApe& BionicApeSubsystem;
	TMap<FUniqueOfferId, FOnlineStoreOfferRef> AvailableOffers;

private:
	void CreateFakeOffer(const FString& Id, const FString& Title, const FString& Description, int32 Price);
};

using FOnlineStoreBionicApePtr = TSharedPtr<FOnlineStoreV2BionicApe, ESPMode::ThreadSafe>;
using FOnlineStoreBionicApeRef = TSharedRef<FOnlineStoreV2BionicApe, ESPMode::ThreadSafe>;
