// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Customs/CustomStructs.h"
#include "MyBlueprintFunctionLibrary.generated.h"



/**
 * 
 */
UCLASS()
class INKGUARD_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

	UFUNCTION(BlueprintCallable, Category = "Additional Packet")
	static const FVaultingPacket BuildVaultingPacket(FVector VaultingStart, FVector VaultingMiddle, FVector VaultingEnd);
};
