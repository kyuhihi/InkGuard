// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../MyNetWorking/Headers/Include.h"
#include "../MyNetWorking/MyNetworkMgr.h"
#include "SpawnMgr.generated.h"

//아래 클래스는 매니저 이긴한데 main map에 배치되는 액터임. 
//무언가 호출하려 결심한다면 해당 객체가 메인맵에서 먼저 생성되었을지 고민해봐야함.

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

public://Spawner Call
	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void RegisterSpawner(ASoldierSpawner* pCallSoldierSpawner, const int iSpawnerColor, const int iTargetTerritory); //지금 caller 스포너가 스폰해야할 객체를 물어보고 가져오는 코드.

public:
	static void Initialize();
	static void SetSoldierInfo(int iIndex, int iSoldierType, int iTargetTerritory);
	static void SetOtherSoldierInfo(const S2C_PACKET_GAMESTART& tGameStartPacket);

private:
	TArray<ASoldierSpawner*> m_pSpawners;
	
private:
	MyNetworkMgr* m_pNetworkMgr = nullptr;
	class AInkGuardGameMode* m_pInkGuardGameMode = nullptr;
};
