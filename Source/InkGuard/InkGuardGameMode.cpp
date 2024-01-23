// Copyright Epic Games, Inc. All Rights Reserved.

#include "InkGuardGameMode.h"
#include "MyNetWorking/MyNetworkMgr.h"
#include "InkGuardCharacter.h"
#include "UObject/ConstructorHelpers.h"


AInkGuardGameMode::AInkGuardGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

AInkGuardGameMode::~AInkGuardGameMode()
{
}

void AInkGuardGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	MyNetworkMgr::GetInstance();
}

void AInkGuardGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	MyNetworkMgr::GetInstance()->DestroyInstance();
}
