// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../MyNetWorking/Headers/Include.h"
#include "../MyNetWorking/MyNetworkMgr.h"
#include "SpawnMgr.generated.h"

//�Ʒ� Ŭ������ �Ŵ��� �̱��ѵ� main map�� ��ġ�Ǵ� ������. 
//���� ȣ���Ϸ� ����Ѵٸ� �ش� ��ü�� ���θʿ��� ���� �����Ǿ����� ����غ�����.

UCLASS()
class INKGUARD_API ASpawnMgr : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnMgr();
	~ASpawnMgr();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	bool GetSoldierData(C2S_PACKET_SOLDIER_TRANSFORM* pSendPacket);	//���ϰ����� Ȥ�� ��ü�� ���ٴ���.. �ϴ��� üũ.
	bool SetSoldierData(const C2S_PACKET_SOLDIER_TRANSFORM* pRecvPacket, const int iStartIndex);	//���ϰ����� Ȥ�� ��ü�� ���ٴ���.. �ϴ��� üũ.

public://Spawner Call
	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void RegisterSpawner(ASoldierSpawner* pCallSoldierSpawner, const int iSpawnerColor, const int iTargetTerritory); //���� caller �����ʰ� �����ؾ��� ��ü�� ����� �������� �ڵ�.
	
	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	FPACKET_SOLDIER_TRANSFORM_BLUEPRINT GetOtherData(const int iSpawnMgrIndex); //���� caller �����ʰ� �����ؾ��� ��ü�� ����� �������� �ڵ�.

public:
	static void Initialize();
	static void SetSoldierInfo(int iIndex, int iSoldierType, int iTargetTerritory);
	static void SetOtherSoldierInfo(const S2C_PACKET_GAMESTART& tGameStartPacket);

private:
	TArray<ASoldierSpawner*> m_pSpawners;		//�츮�� ������ ��ü.
	TArray<ASoldierSpawner*> m_pOtherSpawners;	//�� ������ ��ü.
	
	FPACKET_SOLDIER_TRANSFORM_BLUEPRINT m_OtherSoldiersTransform[SOLDIER_MAX_CNT];

private:
	MyNetworkMgr* m_pNetworkMgr = nullptr;
	class AInkGuardGameMode* m_pInkGuardGameMode = nullptr;
	bool m_bInitialized = false;
};
