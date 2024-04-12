// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnMgr.h"

// Sets default values
ASpawnMgr::ASpawnMgr()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

ASpawnMgr::~ASpawnMgr()
{
	m_pNetworkMgr = nullptr;
}

// Called when the game starts or when spawned
void ASpawnMgr::BeginPlay()
{
	Super::BeginPlay();
	m_pNetworkMgr = MyNetworkMgr::GetInstance();

}

// Called every frame
void ASpawnMgr::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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