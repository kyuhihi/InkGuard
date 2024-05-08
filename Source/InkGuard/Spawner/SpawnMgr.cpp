// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnMgr.h"
#include "SoldierSpawner.h"
#include "../InkGuardGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawnMgr::ASpawnMgr()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

ASpawnMgr::~ASpawnMgr()
{
	m_pNetworkMgr = nullptr;
	m_pInkGuardGameMode = nullptr;

	for (auto& pSpawner : m_pSpawners)
		pSpawner = nullptr;
	m_pSpawners.Empty(0);
}

// Called when the game starts or when spawned
void ASpawnMgr::BeginPlay()
{
	Super::BeginPlay();
	m_pNetworkMgr = MyNetworkMgr::GetInstance();
	m_pInkGuardGameMode = Cast<AInkGuardGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

// Called every frame
void ASpawnMgr::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnMgr::RegisterSpawner(ASoldierSpawner* pCallSoldierSpawner, const int iSpawnerColor, const int iTargetTerritory)
{
	m_pSpawners.Add(pCallSoldierSpawner);
	EMainGameState eGameMode = m_pInkGuardGameMode->GetCurGameMode();
	
	switch (eGameMode)
	{
	case EMainGameState::GAME_WAITING:
		return;
	case EMainGameState::GAME_MAINGAME:
		break;
	default:
		break;
	}

	GAME_PLAY eSpawnerColor = (GAME_PLAY)iSpawnerColor;



	bool bTeamRedColor{ true };
	m_pInkGuardGameMode->GetTeamColor(bTeamRedColor);
	


	GAME_PLAY eTeamColor = GAME_PLAY::GAME_BLUE_TEAM;
	if (bTeamRedColor)
		eTeamColor = GAME_RED_TEAM;

	if (eTeamColor == eSpawnerColor)
	{// 내팀 컬러와 같으면 내거 뒤지고 내 팀컬러와 다르면 적팀 솔져정보 뒤짐.
		for (int i = 0; i < SOLDIER_MAX_CNT; i++)
		{
			if (iTargetTerritory == MyNetworkMgr::m_tSoldierInfo[i].eTargetTerritory)
			{
				pCallSoldierSpawner->AppendNewDuty(MyNetworkMgr::m_tSoldierInfo[i].eSoldierType);
			}
		}
	}
	else
	{
		for (int i = 0; i < SOLDIER_MAX_CNT; i++)
		{
			if (iTargetTerritory == MyNetworkMgr::m_tOtherSoldierInfo[i].eTargetTerritory)
			{
				pCallSoldierSpawner->AppendNewDuty(MyNetworkMgr::m_tOtherSoldierInfo[i].eSoldierType);
			}
		}
	}

}

#pragma region static func

void ASpawnMgr::Initialize()
{
	for (int i = 0; i < SOLDIER_MAX_CNT; i++)
	{
		MyNetworkMgr::m_tSoldierInfo[i].eSoldierType = (SOLDIER_TYPE)(i % SOLDIER_TYPE::SOLDIER_END);
		MyNetworkMgr::m_tSoldierInfo[i].eTargetTerritory = (TERRITORY_TYPE)(i % TERRITORY_TYPE::TERRITORY_END);
	}
}

void ASpawnMgr::SetSoldierInfo(int iIndex, int iSoldierType, int iTargetTerritory)
{
	MyNetworkMgr::m_tSoldierInfo[iIndex].eSoldierType = (SOLDIER_TYPE)iSoldierType;
	MyNetworkMgr::m_tSoldierInfo[iIndex].eTargetTerritory = (TERRITORY_TYPE)iTargetTerritory;
}

void ASpawnMgr::SetOtherSoldierInfo(const S2C_PACKET_GAMESTART& tGameStartPacket)
{
	for (int i = 0; i < SOLDIER_MAX_CNT; ++i)
	{//받은 상대편 패킷으로 저장.
		MyNetworkMgr::m_tOtherSoldierInfo[i].eSoldierType = (SOLDIER_TYPE)tGameStartPacket.cOtherSoldierInfo[i];
		MyNetworkMgr::m_tOtherSoldierInfo[i].eTargetTerritory = (TERRITORY_TYPE)tGameStartPacket.cOtherTargetTerritory[i];
	}
}

#pragma endregion