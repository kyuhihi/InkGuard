// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Customs/CustomStructs.h"
#include "PlayerInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INKGUARD_API IPlayerInteractionInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Check")
	EAdditionalPacketType GetInteractionType();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Check")
	bool IsInteraction();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Send")
	void GetSendDutyData(FVector& Value1, FVector& Value2, FVector& Value3, EAdditionalPacketType& InteractionType);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Recv")
	void SetReceivedDutyData(FVector Value1, FVector Value2, FVector Value3);
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
};
