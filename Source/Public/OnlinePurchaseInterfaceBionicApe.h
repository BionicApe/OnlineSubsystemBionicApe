// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Interfaces/OnlinePurchaseInterface.h"
#include "OnlineSubsystemBionicApeTypes.h"

class FOnlineSubsystemBionicApe;

class FOnlinePurchaseBionicApe
	: public IOnlinePurchase
	, public TSharedFromThis<FOnlinePurchaseBionicApe, ESPMode::ThreadSafe>
{
public:
	FOnlinePurchaseBionicApe(FOnlineSubsystemBionicApe& InBionicApeSubsystem);
	virtual ~FOnlinePurchaseBionicApe();

	void Tick();

public:
	//~ Begin IOnlinePurchase Interface
	virtual bool IsAllowedToPurchase(const FUniqueNetId& UserId) override;
	virtual void Checkout(const FUniqueNetId& UserId, const FPurchaseCheckoutRequest& CheckoutRequest, const FOnPurchaseCheckoutComplete& Delegate) override;
	virtual void FinalizePurchase(const FUniqueNetId& UserId, const FString& ReceiptId) override;
	virtual void RedeemCode(const FUniqueNetId& UserId, const FRedeemCodeRequest& RedeemCodeRequest, const FOnPurchaseRedeemCodeComplete& Delegate) override;
	virtual void QueryReceipts(const FUniqueNetId& UserId, bool bRestoreReceipts, const FOnQueryReceiptsComplete& Delegate) override;
	virtual void GetReceipts(const FUniqueNetId& UserId, TArray<FPurchaseReceipt>& OutReceipts) const override;
	virtual void FinalizeReceiptValidationInfo(const FUniqueNetId& UserId, FString& InReceiptValidationInfo, const FOnFinalizeReceiptValidationInfoComplete& Delegate) override;
	//~ End IOnlinePurchase Interface

PACKAGE_SCOPE:
	void CheckoutSuccessfully(const FUniqueNetIdBionicApe& UserId, TSharedPtr<FOnlineStoreOffer> Offer);

PACKAGE_SCOPE:
	/** Pointer back to our parent subsystem */
	FOnlineSubsystemBionicApe& BionicApeSubsystem;

	/** Cached receipts information per user */
	TMap<FUniqueNetIdBionicApe, TArray<FPurchaseReceipt> > UserFakeReceipts;

	/** Do we have a purchase currently in progress? */
	TOptional<FOnPurchaseCheckoutComplete> PendingPurchaseDelegate;

	TOptional<double> PendingPurchaseFailTime;
};

using FOnlinePurchaseBionicApePtr = TSharedPtr<FOnlinePurchaseBionicApe, ESPMode::ThreadSafe>;
using FOnlinePurchaseBionicApeRef = TSharedRef<FOnlinePurchaseBionicApe, ESPMode::ThreadSafe>;
