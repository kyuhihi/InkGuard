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
	APawn* pPlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fVelocityZ = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fSpeed = 0.f;
};

USTRUCT(Atomic, BlueprintType)
struct FPlayerInputStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PressingF = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Attack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Vault = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Dodge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Sprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MontagePlayTime = 0.f;
};
