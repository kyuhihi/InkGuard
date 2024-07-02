// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnMgr.h"
#include "SoldierSpawner.h"
#include "../InkGuardGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "../Customs/CustomFunctional.h"
#include "SoldierInterface.h"

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

	m_pNetworkMgr->RegisterSpawnMgr(this);
}

// Called every frame
void ASpawnMgr::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ASpawnMgr::GetSoldierData(C2S_PACKET_SOLDIER_TRANSFORM* pSendPacket)
{
	if (m_bInitialized == false)
		return false;

	for (auto& OurTeamSpawner : m_pSpawners)
	{
		const TArray<FSpawnDutyStruct>& SpawnerInfoArray = OurTeamSpawner->GetSpawnerInfo();
		if (SpawnerInfoArray.Num() == 0)
			continue;

		for (auto& Soldier : SpawnerInfoArray)
		{
			//ACharacter* pCharacter = Soldier.pTargetActor.Get();
			//USkeletalMeshComponent* pMesh = pCharacter->GetMesh();
			USkeletalMeshComponent* pMesh = Soldier.pTargetActor->GetMesh();
			UAnimInstance* pAnimInstance = pMesh->GetAnimInstance();

			UAnimMontage* pAnimMontage = pAnimInstance->GetCurrentActiveMontage();
			float fPlayTime(0.f);
			if (IsValid(pAnimMontage) != false)
			{
				//UE_LOG(InkGuardNetErr, Log, TEXT("[X]: %f [Y]: %f [Z]: %f"), tRecvPacket.vPosition.x, tRecvPacket.vPosition.y, tRecvPacket.vPosition.z);
				fPlayTime = pAnimInstance->Montage_GetPosition(pAnimMontage);
			}

			FVector Velocity = Soldier.pTargetActor.Get()->GetVelocity();
			FVector Position = Soldier.pTargetActor.Get()->GetActorLocation();
			FRotator tPlayerRotation{ Soldier.pTargetActor->GetActorRotation() };

			for (int i = 0; i < SOLDIER_MAX_CNT; i++) {

				if (Soldier.iSpawnMgrIndex == i)
				{
					pSendPacket[i].fSoldier_MontagePlayTime = fPlayTime;
					pSendPacket[i].fSoldier_Speed = (float)Velocity.Length();
					pSendPacket[i].fSoldier_Yaw = tPlayerRotation.Yaw;
					pSendPacket[i].fHP = Soldier.fSoldierHP;
					pSendPacket[i].vSoldier_Position = UCustomFunctional::FVector_To_float3(Position);
					break;
				}
			}

		}
	}

	return true;
}

bool ASpawnMgr::SetSoldierData(const C2S_PACKET_SOLDIER_TRANSFORM* pRecvPacket, const int iStartIndex)
{
	if (m_bInitialized == false)
		return false;
	int iEndIndex = 4; //디폴트로 0~4 임.
	if (iStartIndex == 4)
	{//0,1,2,3		//4,5,6,7,8
		iEndIndex = SOLDIER_MAX_CNT;
	}

	for (int i = 0; i < SOLDIER_MAX_CNT; ++i)
	{
		m_OtherSoldiersUpdate[i] = false;
	}

	for (int i = iStartIndex; i < iEndIndex; i++)
	{
			FPACKET_SOLDIER_TRANSFORM_BLUEPRINT tCastedPacket;
			tCastedPacket.fSoldier_MontagePlayTime = pRecvPacket[i].fSoldier_MontagePlayTime;
			tCastedPacket.fSoldier_Speed = pRecvPacket[i].fSoldier_Speed;
			tCastedPacket.fSoldier_Yaw = pRecvPacket[i].fSoldier_Yaw;
			tCastedPacket.vSoldier_Position = UCustomFunctional::float3_To_FVector(pRecvPacket[i].vSoldier_Position);
			m_OtherSoldiersTransform[i] = tCastedPacket;
			m_OtherSoldiersUpdate[i] = true;
	}

	return true;
}

void ASpawnMgr::RegisterSpawner(ASoldierSpawner* pCallSoldierSpawner, const int iSpawnerColor, const int iTargetTerritory)
{
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
		m_pSpawners.Add(pCallSoldierSpawner);
		for (int i = 0; i < SOLDIER_MAX_CNT; i++)
		{
			if (iTargetTerritory == MyNetworkMgr::m_tSoldierInfo[i].eTargetTerritory)
			{
				pCallSoldierSpawner->AppendNewDuty((int)MyNetworkMgr::m_tSoldierInfo[i].eSoldierType, i);
			}
		}
	}
	else
	{
		m_pOtherSpawners.Add(pCallSoldierSpawner);
		for (int i = 0; i < SOLDIER_MAX_CNT; i++)
		{
			if (iTargetTerritory == MyNetworkMgr::m_tOtherSoldierInfo[i].eTargetTerritory)
			{
				pCallSoldierSpawner->AppendNewDuty((int)MyNetworkMgr::m_tOtherSoldierInfo[i].eSoldierType,i);
			}
		}
	}

	m_bInitialized = true;

}

FPACKET_SOLDIER_TRANSFORM_BLUEPRINT ASpawnMgr::GetOtherData(const int iSpawnMgrIndex)
{
	return m_OtherSoldiersTransform[iSpawnMgrIndex];
}

bool ASpawnMgr::IsOtherDataUpdate(const int iSpawnMgrIndex) const
{
	return m_OtherSoldiersUpdate[iSpawnMgrIndex];
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