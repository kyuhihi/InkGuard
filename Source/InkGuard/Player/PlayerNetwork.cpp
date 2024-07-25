// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerNetwork.h"
#include "../MyNetWorking/MyNetworkMgr.h"
#include "GameFramework/Character.h"
#include "../Customs/CustomFunctional.h"
#include "../MyBlueprintFunctionLibrary.h"

// Sets default values for this component's properties
UPlayerNetwork::UPlayerNetwork()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

UPlayerNetwork::~UPlayerNetwork()
{
	m_pNetworkMgr = nullptr;
}


// Called when the game starts
void UPlayerNetwork::BeginPlay()
{
	Super::BeginPlay();
	m_pNetworkMgr = MyNetworkMgr::GetInstance();
	
}

// Called every frame
void UPlayerNetwork::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//UE_LOG(InkGuardNetErr, Warning, TEXT("%f"), DeltaTime);

	//m_fSyncTimer += DeltaTime;
	
	if (!m_pNetworkMgr)
	{
		m_pNetworkMgr = MyNetworkMgr::GetInstance();
	}

	//CheckGameStart();

}

#pragma region Packet

bool UPlayerNetwork::CheckGameStart()
{
	if (m_pNetworkMgr->GetGameStart() && m_pNetworkMgr->GetReservedOpenLevel())// 게임 시작이 되었는지. 오픈레벨이 걸린상황은 아닌지.
		return true; 

	
	m_pNetworkMgr->SendGameStart();
	m_pNetworkMgr->RecvGameStart();


	
	return false;
}

void UPlayerNetwork::SendPlayerTransform(float DeltaTime)
{
	if (!m_pNetworkMgr->GetGameStart())
		return;

	m_fSyncTimer += DeltaTime;

	FVector vPlayerPosition{ m_tPlayerStruct.pPlayer->GetActorLocation() };
	FRotator tPlayerRotation{ m_tPlayerStruct.pPlayer->GetActorRotation() };
	FVector vPlayerVelocity{ m_tPlayerStruct.pPlayer->GetVelocity() };

	
	
	if (m_fSyncTimer > SERVER_SYNC_TIME){
		m_pNetworkMgr->SetSyncTime(true);
	}
	
	m_pNetworkMgr->SendPlayerTransform(vPlayerPosition, tPlayerRotation, vPlayerVelocity.Z, vPlayerVelocity.Length(),m_fBakuHealth);

	//UE_LOG(InkGuardNetErr, Warning, TEXT("SendPlayerTransform"));
}

void UPlayerNetwork::RecvPlayerTransform(float DeltaTime)
{
	if (!m_pNetworkMgr->GetSyncTime() || !m_pNetworkMgr->GetGameStart())
		return;

	S2C_PACKET_PLAYER_TRANSFORM tRecvPacket;

	if (!m_pNetworkMgr->RecvPlayerTransform(tRecvPacket))
		return;



	//UE_LOG(InkGuardNetErr, Log, TEXT("[X]: %f [Y]: %f [Z]: %f"), tRecvPacket.vPosition.x, tRecvPacket.vPosition.y, tRecvPacket.vPosition.z);

	GetOwner()->SetActorLocation(UCustomFunctional::float3_To_FVector(tRecvPacket.vPosition),false,nullptr,ETeleportType::ResetPhysics);

	FRotator PlayerRotation(GetOwner()->GetActorRotation());
	PlayerRotation.Yaw = tRecvPacket.fYaw;
	GetOwner()->SetActorRotation(PlayerRotation);
	m_tPlayerStruct.fSpeed = tRecvPacket.fSpeed;
	m_tPlayerStruct.fVelocityZ= tRecvPacket.fVelocityZ;
	m_tPlayerStruct.fHP = tRecvPacket.fHP;

	m_fSyncTimer = 0.f;// recv해야하는 타이밍인지 초기화.

	//UE_LOG(InkGuardNetErr, Warning, TEXT("RecvPlayerTransform"));

}

void UPlayerNetwork::SendPlayerInputData(float DeltaTime, const FPlayerInputStruct& tBakuInputData)
{ 
	C2S_PACKET_PLAYER_INPUT tSendPacket;

	tSendPacket.bInputs[PLAYER_INPUT::INPUT_F] = tBakuInputData.PressingF;
	tSendPacket.bInputs[PLAYER_INPUT::INPUT_ATTACK] = tBakuInputData.Attack;
	tSendPacket.bInputs[PLAYER_INPUT::INPUT_DODGE] = tBakuInputData.Dodge;
	tSendPacket.bInputs[PLAYER_INPUT::INPUT_SPRINT] = tBakuInputData.Sprint;
	tSendPacket.bInputs[PLAYER_INPUT::INPUT_CROUCH] = tBakuInputData.Crouch;
	tSendPacket.bInputs[PLAYER_INPUT::INPUT_QSKILL] = tBakuInputData.QSkill;
	tSendPacket.bInputs[PLAYER_INPUT::INPUT_ESKILL] = tBakuInputData.ESkill;
	
	tSendPacket.fMontagePlayTime = tBakuInputData.MontagePlayTime;
	tSendPacket.cInteractionType = (uint8)tBakuInputData.InteractionType;

	tSendPacket.tAddPacket.vValue1 = UCustomFunctional::FVector_To_float3(tBakuInputData.Value1);
	tSendPacket.tAddPacket.vValue2 = UCustomFunctional::FVector_To_float3(tBakuInputData.Value2);
	tSendPacket.tAddPacket.vValue3 = UCustomFunctional::FVector_To_float3(tBakuInputData.Value3);



	//SendPlayerInputData
	m_pNetworkMgr->SendPlayerInputData(tSendPacket);

	//UE_LOG(InkGuardNetErr, Warning, TEXT("SendPlayerInputData"));

}

void UPlayerNetwork::RecvPlayerInputData(float DeltaTime, FPlayerInputStruct& tOutInputs)
{//추가 패킷사이즈는 플레이어
	S2C_PACKET_PLAYER_INPUT tRecvPacket;

	if (m_pNetworkMgr->RecvPlayerInputData(tRecvPacket))
	{
		tOutInputs.PressingF = tRecvPacket.bInputs[PLAYER_INPUT::INPUT_F];
		tOutInputs.Attack = tRecvPacket.bInputs[PLAYER_INPUT::INPUT_ATTACK];
		tOutInputs.Dodge = tRecvPacket.bInputs[PLAYER_INPUT::INPUT_DODGE];
		tOutInputs.Sprint = tRecvPacket.bInputs[PLAYER_INPUT::INPUT_SPRINT];
		tOutInputs.Crouch = tRecvPacket.bInputs[PLAYER_INPUT::INPUT_CROUCH];
		tOutInputs.QSkill = tRecvPacket.bInputs[PLAYER_INPUT::INPUT_QSKILL];
		tOutInputs.ESkill = tRecvPacket.bInputs[PLAYER_INPUT::INPUT_ESKILL];

		tOutInputs.MontagePlayTime = tRecvPacket.fMontagePlayTime;
		tOutInputs.InteractionType = (EAdditionalPacketType)tRecvPacket.cInteractionType;

		tOutInputs.Value1 = UCustomFunctional::float3_To_FVector(tRecvPacket.tAddPacket.vValue1);
		tOutInputs.Value2 = UCustomFunctional::float3_To_FVector(tRecvPacket.tAddPacket.vValue2);
		tOutInputs.Value3 = UCustomFunctional::float3_To_FVector(tRecvPacket.tAddPacket.vValue3);
	}
	//UE_LOG(InkGuardNetErr, Warning, TEXT("RecvPlayerInputData"));
	
	return;
}

void UPlayerNetwork::SendAdditionalData(float DeltaTime)
{
	//if (m_pNetworkMgr->GetAdditionalSendPacketSize() > 0) {
	//	int iSendByte = m_pNetworkMgr->SendAdditionalData();
	//	if (iSendByte > 0)
	//	{
	//		UE_LOG(InkGuardNetErr, Warning, TEXT("Additional SendByte %d"), iSendByte);
	//	}
	//}
}

void UPlayerNetwork::RecvAdditionalData(float DeltaTime)
{
	//m_pNetworkMgr->RecvAdditionalData();
}

const FVaultingPacket UPlayerNetwork::GetVaultingData()
{
	C2S_PACKET_ADDITIONAL_FLOAT3x3 tFindData;
	//m_pNetworkMgr->FindAdditionalData(EAdditionalPacketType::ADD_VAULT, tFindData);
	FVaultingPacket RetData;
	RetData.VaultingStart = UCustomFunctional::float3_To_FVector(tFindData.vValue1);
	RetData.VaultingMiddle = UCustomFunctional::float3_To_FVector(tFindData.vValue2);
	RetData.VaultingEnd= UCustomFunctional::float3_To_FVector(tFindData.vValue3);
	return RetData;
}

#pragma endregion

void UPlayerNetwork::SetPlayerStruct(FPlayerStruct tPlayerData)
{
	m_tPlayerStruct = tPlayerData;
}

const FPlayerStruct& UPlayerNetwork::GetPlayerStruct()
{
	return m_tPlayerStruct;
}

void UPlayerNetwork::SetPlayerHpForSendTransform(float fNewHP)
{
	m_fBakuHealth = fNewHP;
}



void UPlayerNetwork::TidyNetworkTickRoutine()
{
	m_pNetworkMgr->SetSyncTime(false);
	m_pNetworkMgr->ClearAdditionalPacket();

}


void UPlayerNetwork::AddAdditionalData(const FVaultingPacket tNewPacket)
{	
	C2S_PACKET_ADDITIONAL_FLOAT3x3 tCast2Packet;
	
	tCast2Packet.vValue1 = UCustomFunctional::FVector_To_float3(tNewPacket.VaultingStart);
	tCast2Packet.vValue2 = UCustomFunctional::FVector_To_float3(tNewPacket.VaultingMiddle);
	tCast2Packet.vValue3 = UCustomFunctional::FVector_To_float3(tNewPacket.VaultingEnd);

	m_pNetworkMgr->AppendDataToAdditionalList(true, tNewPacket.ePacketType, tCast2Packet);
}
