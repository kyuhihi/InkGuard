// Copyright Epic Games, Inc. All Rights Reserved.

#include "InkGuardGameMode.h"
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
