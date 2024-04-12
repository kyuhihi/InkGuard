// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyNetWorking/Include.h"
#include "InkGuardGameMode.generated.h"

class MyNetworkMgr; 

UENUM(BlueprintType)
enum class EGameState : uint8
{
	GAME_WAITING UMETA(DisplayName = "GAME_WAITING"),
	GAME_MAINGAME UMETA(DisplayName = "GAME_MAINGAME"),
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
	void SetSoldierInfo(int iIndex,int iSoldierType, int iTargetTerritory);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void SetReservedOpenLevel(bool bNewValue);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	EGameState GetCurGameMode();

private:
	MyNetworkMgr* m_pNetWorkMgr = nullptr;

};



