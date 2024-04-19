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
	
	m_pNetworkMgr->SendPlayerTransform(vPlayerPosition, tPlayerRotation, vPlayerVelocity.Z, vPlayerVelocity.Length());

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

	m_fSyncTimer = 0.f;// recv해야하는 타이밍인지 초기화.

	//UE_LOG(InkGuardNetErr, Warning, TEXT("RecvPlayerTransform"));

}

void UPlayerNetwork::SendPlayerInputData(float DeltaTime, const FPlayerInputStruct& tBakuInputData)
{ 
	C2S_PACKET_PLAYER_INPUT tSendPacket;

	tSendPacket.bInputs[PLAYER_INPUT::INPUT_F] = tBakuInputData.PressingF;
	tSendPacket.bInputs[PLAYER_INPUT::INPUT_ATTACK] = tBakuInputData.Attack;
	tSendPacket.bInputs[PLAYER_INPUT::INPUT_VAULT] = tBakuInputData.Vault;
	tSendPacket.bInputs[PLAYER_INPUT::INPUT_DODGE] = tBakuInputData.Dodge;
	tSendPacket.bInputs[PLAYER_INPUT::INPUT_SPRINT] = tBakuInputData.Sprint;
	tSendPacket.fMontagePlayTime = tBakuInputData.MontagePlayTime;

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
		tOutInputs.Vault = tRecvPacket.bInputs[PLAYER_INPUT::INPUT_VAULT];
		tOutInputs.Dodge = tRecvPacket.bInputs[PLAYER_INPUT::INPUT_DODGE];
		tOutInputs.Sprint = tRecvPacket.bInputs[PLAYER_INPUT::INPUT_SPRINT];
		tOutInputs.MontagePlayTime = tRecvPacket.fMontagePlayTime;

	}
	//UE_LOG(InkGuardNetErr, Warning, TEXT("RecvPlayerInputData"));
	
	return;
}

void UPlayerNetwork::SendAdditionalData(float DeltaTime)
{


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



void UPlayerNetwork::TidyNetworkTickRoutine()
{
	m_pNetworkMgr->SetSyncTime(false);

}


void UPlayerNetwork::AddAdditionalData(const FVaultingPacket tNewPacket)
{	
	C2S_PACKET_ADDITIONAL_FLOAT3x3 tCast2Packet;
	
	tCast2Packet.vValue1 = UCustomFunctional::FVector_To_float3(tNewPacket.VaultingStart);
	tCast2Packet.vValue2 = UCustomFunctional::FVector_To_float3(tNewPacket.VaultingMiddle);
	tCast2Packet.vValue3 = UCustomFunctional::FVector_To_float3(tNewPacket.VaultingEnd);

	m_pNetworkMgr->AppendDataToAdditionalList(true, tNewPacket.ePacketType, tCast2Packet);
}
