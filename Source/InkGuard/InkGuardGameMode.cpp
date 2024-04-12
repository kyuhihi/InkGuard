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
}

AInkGuardGameMode::~AInkGuardGameMode()
{//씬 게임모드도 삭제됨.

}

void AInkGuardGameMode::Initialize()
{
}

void AInkGuardGameMode::SetSoldierInfo(int iIndex, int iSoldierType, int iTargetTerritory)
{
	if (m_pNetWorkMgr == nullptr)
		m_pNetWorkMgr = MyNetworkMgr::GetInstance(); 


	m_pNetWorkMgr->SetSoldierInfo(iIndex, iSoldierType, iTargetTerritory);

}

void AInkGuardGameMode::SetReservedOpenLevel(bool bNewValue)
{
	if (m_pNetWorkMgr == nullptr)
		m_pNetWorkMgr = MyNetworkMgr::GetInstance(); 

	m_pNetWorkMgr->SetReservedOpenLevel(bNewValue); //메인레벨의 경우에만 set false를 걸어준다.
}

EGameState AInkGuardGameMode::GetCurGameMode()
{
	if (m_pNetWorkMgr == nullptr)
		m_pNetWorkMgr = MyNetworkMgr::GetInstance();
	
	if ((m_pNetWorkMgr->GetGameStart() == true))
	{
		if (m_pNetWorkMgr->GetReservedOpenLevel() == true)
		{
			return EGameState::GAME_WAITING;
		}
		//false라면 제대로 시작된 것.
		return EGameState::GAME_MAINGAME;
	}

	return EGameState::GAME_WAITING;
}
