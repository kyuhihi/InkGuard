// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyNetWorking/Include.h"
#include "InkGuardGameMode.generated.h"

class MyNetworkMgr; 

UCLASS(minimalapi)
class AInkGuardGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AInkGuardGameMode();
	~AInkGuardGameMode();

public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);
	virtual void Logout(AController* Exiting);
	void Initialize();

public:
	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void OpenMainGame();

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void SetSoldierInfo(int iIndex,int iSoldierType, int iTargetTerritory);


private:
	MyNetworkMgr* m_pNetWorkMgr = nullptr;
};



