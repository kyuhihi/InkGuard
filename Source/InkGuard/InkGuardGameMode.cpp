// Copyright Epic Games, Inc. All Rights Reserved.

#include "InkGuardGameMode.h"
#include "MyNetWorking/MyNetworkMgr.h"
#include "InkGuardCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Level.h"
#include "Engine/World.h"


AInkGuardGameMode::AInkGuardGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	Initialize();
}

AInkGuardGameMode::~AInkGuardGameMode()
{
}

void AInkGuardGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	//m_pNetWorkMgr = MyNetworkMgr::GetInstance();

}

void AInkGuardGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	//MyNetworkMgr::GetInstance()->DestroyInstance();

	m_pNetWorkMgr = nullptr;
}

void AInkGuardGameMode::Initialize()
{
	for (int i = 0; i < SOLDIER_MAX_CNT; ++i) {
		m_SoldierInfo[i].eSoldierType = (SOLDIER_TYPE)(i / 2);
		m_SoldierInfo[i].eTargetTerritory = (TERRITORY_TYPE)(i / 3);
	}
}

void AInkGuardGameMode::OpenMainGame()
{
}

void AInkGuardGameMode::SetSoldierInfo(int iIndex, int iSoldierType, int iTargetTerritory)
{
	if (SOLDIER_MAX_CNT < iSoldierType)
		return;

	m_SoldierInfo[iIndex].eSoldierType = (SOLDIER_TYPE)iSoldierType;
	m_SoldierInfo[iIndex].eTargetTerritory = (TERRITORY_TYPE)iTargetTerritory;
}
