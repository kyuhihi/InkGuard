// Copyright Epic Games, Inc. All Rights Reserved.

#include "InkGuardGameMode.h"
#include "MyNetWorking/MyNetworkMgr.h"
#include "Default/InkGuardCharacter.h"
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
	m_pNetWorkMgr = nullptr;
}

void AInkGuardGameMode::Initialize()
{
}

void AInkGuardGameMode::ModifyIP(FString NewIPAddr)
{
	MyNetworkMgr::SERVERIP = TCHAR_TO_UTF8(*NewIPAddr);
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

EMainGameState AInkGuardGameMode::GetCurGameMode()
{
	if (m_pNetWorkMgr == nullptr)
		m_pNetWorkMgr = MyNetworkMgr::GetInstance();
	
	if ((m_pNetWorkMgr->GetGameStart() == true))
	{
		if (m_pNetWorkMgr->GetReservedOpenLevel() == true)
		{
			return EMainGameState::GAME_WAITING;
		}
		//false��� ����� ���۵� ��.
		return EMainGameState::GAME_MAINGAME;
	}

	return EMainGameState::GAME_WAITING;
}

void AInkGuardGameMode::GetTeamColor(bool& bRedColor)
{
	if (m_pNetWorkMgr == nullptr)
		m_pNetWorkMgr = MyNetworkMgr::GetInstance();

	GAME_PLAY eTeamColor = m_pNetWorkMgr->GetTeamColor();
	if (eTeamColor == GAME_RED_TEAM)
		bRedColor = true;
	else
		bRedColor = false;
}
