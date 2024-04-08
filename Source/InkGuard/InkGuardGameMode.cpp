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

	MyNetworkMgr::GetInstance()->DestroyInstance();

	m_pNetWorkMgr = nullptr;
}

void AInkGuardGameMode::Initialize()
{

}

void AInkGuardGameMode::OpenMainGame()
{
	if(m_pNetWorkMgr == nullptr)
		m_pNetWorkMgr = MyNetworkMgr::GetInstance(); //connect작업 할거임.
	// 스레드 생성
	m_pNetWorkMgr->OpenMainGame();// 여기서 쓰레드 생성해서 게임 시작했는지 체크함.
}

void AInkGuardGameMode::SetSoldierInfo(int iIndex, int iSoldierType, int iTargetTerritory)
{
	if (m_pNetWorkMgr == nullptr)
		m_pNetWorkMgr = MyNetworkMgr::GetInstance(); //connect작업 할거임.


	m_pNetWorkMgr->SetSoldierInfo(iIndex, iSoldierType, iTargetTerritory);

}

EGameState AInkGuardGameMode::GetCurGameMode()
{
	if (m_eGameMode == EGameState::GAME_MAINGAME)
		return EGameState::GAME_MAINGAME;

	m_pNetWorkMgr->SetGameStartMutexLock();
	bool bGameStart = m_pNetWorkMgr->GetGameStart();
	m_pNetWorkMgr->SetGameStartMutexUnLock();
	if (bGameStart)
	{
		m_eGameMode = EGameState::GAME_MAINGAME;
	}

	return m_eGameMode;
}
