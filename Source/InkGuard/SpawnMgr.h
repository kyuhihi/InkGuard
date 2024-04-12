// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyNetWorking/Include.h"
#include "MyNetWorking/MyNetworkMgr.h"
#include "SpawnMgr.generated.h"

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
	static void Initialize();
	static void SetSoldierInfo(int iIndex, int iSoldierType, int iTargetTerritory);
	static void SetOtherSoldierInfo(const S2C_PACKET_GAMESTART& tGameStartPacket);

private:
	MyNetworkMgr* m_pNetworkMgr = nullptr;
	//SOLDIERINFO m_tSoldierInfo[SOLDIER_MAX_CNT];			//내꺼 솔져 정보. .. 매번 객체가 레벨 열릴때마다 소멸됨..
	//SOLDIERINFO m_tOtherSoldierInfo[SOLDIER_MAX_CNT];	//상대거 솔져 정보.

};
