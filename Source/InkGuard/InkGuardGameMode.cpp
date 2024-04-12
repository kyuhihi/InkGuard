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
{//�� ���Ӹ�嵵 ������.

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

	m_pNetWorkMgr->SetReservedOpenLevel(bNewValue); //���η����� ��쿡�� set false�� �ɾ��ش�.
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
		//false��� ����� ���۵� ��.
		return EGameState::GAME_MAINGAME;
	}

	return EGameState::GAME_WAITING;
}
