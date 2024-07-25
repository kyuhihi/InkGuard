// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomStructs.generated.h"


UENUM(BlueprintType)
enum class EAdditionalPacketType : uint8
{
	INTERACTION_DORERAE_A UMETA(DisplayName = "INTERACTION_DORERAE_A"),
	INTERACTION_DORERAE_B UMETA(DisplayName = "INTERACTION_DORERAE_B"),
	INTERACTION_DORERAE_C UMETA(DisplayName = "INTERACTION_DORERAE_C"),
	INTERACTION_VAULT UMETA(DisplayName = "INTERACTION_VAULT"),
	INTERACTION_CLIMB UMETA(DisplayName = "INTERACTION_CLIMB"),
	INTERACTION_DAEPO_RED UMETA(DisplayName = "INTERACTION_DAEPO_RED"),
	INTERACTION_DAEPO_BLUE UMETA(DisplayName = "INTERACTION_DAEPO_BLUE"),
	INTERACTION_BALISTA UMETA(DisplayName = "INTERACTION_BALISTA"),
	INTERACTION_END UMETA(DisplayName = "INTERACTION_END"),
};

/**
 *
 */

UCLASS()
class INKGUARD_API UCustomStructs : public UObject
{
	GENERATED_BODY()
};

USTRUCT(Atomic, BlueprintType)
struct FSpawnDutyStruct
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int iSpawnType = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int iSpawnMgrIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fSoldierHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int iLastDamageCauser = -1;
};

USTRUCT(Atomic, BlueprintType)
struct FPACKET_SOLDIER_TRANSFORM_BLUEPRINT
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector	vSoldier_Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		fSoldier_Speed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		fSoldier_MontagePlayTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		fSoldier_Yaw = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		fHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int			iLastDamageCauser = -1;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fHP = 100.f;
};

USTRUCT(Atomic, BlueprintType)
struct FPlayerInputStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAdditionalPacketType InteractionType = EAdditionalPacketType::INTERACTION_END;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PressingF = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Attack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Dodge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Sprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Crouch = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool QSkill = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ESkill = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MontagePlayTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Value1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Value2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Value3;
};

USTRUCT(Atomic, BlueprintType)
struct FAdditionalBase 
{
	GENERATED_USTRUCT_BODY()
public:
	EAdditionalPacketType ePacketType;
};


USTRUCT(Atomic, BlueprintType)
struct FVaultingPacket : public FAdditionalBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector VaultingStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector VaultingMiddle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector VaultingEnd;
	
};