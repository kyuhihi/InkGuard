// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomStructs.generated.h"

/**
 * 
 */
UCLASS()
class INKGUARD_API UCustomStructs : public UObject
{
	GENERATED_BODY()
};


USTRUCT(Atomic, BlueprintType)
struct FPlayerStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* pPlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRedTeam = false;
};
