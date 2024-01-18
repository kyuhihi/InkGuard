// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
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
	virtual void Logout(AController* Exiting) ;

};



