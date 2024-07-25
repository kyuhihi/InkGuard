// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyNetWorking/Headers/Include.h"
#include "InkGuardGameMode.generated.h"

class MyNetworkMgr; 

UENUM(BlueprintType)
enum class EMainGameState : uint8
{
	GAME_WAITING UMETA(DisplayName = "GAME_WAITING"),
	GAME_MAINGAME UMETA(DisplayName = "GAME_MAINGAME"),
};

UENUM(BlueprintType)
enum class EWarpPosType : uint8
{
	WARP_A UMETA(DisplayName = "WARP_A"),
	WARP_B UMETA(DisplayName = "WARP_B"),
	WARP_C UMETA(DisplayName = "WARP_C"),
	WARP_BLUE_SPAWN UMETA(DisplayName = "WARP_BLUE_SPAWN"),
	WARP_RED_SPAWN UMETA(DisplayName = "WARP_RED_SPAWN"),
	WARP_BLUE_BAZOOKA UMETA(DisplayName = "WARP_BLUE_BAZOOKA"),
	WARP_BLUE_BALISTA UMETA(DisplayName = "WARP_BLUE_BALISTA"),
	WARP_BLUE_GONDOLA UMETA(DisplayName = "WARP_BLUE_GONDOLA"),
	WARP_RED_BAZOOKA UMETA(DisplayName = "WARP_RED_BAZOOKA"),
	WARP_RED_BALISTA UMETA(DisplayName = "WARP_RED_BALISTA"),
	WARP_RED_GOONDOLA UMETA(DisplayName = "WARP_RED_GOONDOLA"),
	WARP_END UMETA(DisplayName = "WARP_END"),
};

UCLASS(minimalapi)
class AInkGuardGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AInkGuardGameMode();
	~AInkGuardGameMode();

public:
	void Initialize();

public:
	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void ModifyIP(FString NewIPAddr);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void SetSoldierInfo(int iIndex,int iSoldierType, int iTargetTerritory);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void SetReservedOpenLevel(bool bNewValue);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	EMainGameState GetCurGameMode();

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void GetTeamColor(bool& bRedColor);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	float GetServerTime();

private:
	MyNetworkMgr* m_pNetWorkMgr = nullptr;

};



