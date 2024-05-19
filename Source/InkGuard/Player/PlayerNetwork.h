// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Customs/CustomStructs.h"
#include "../MyNetWorking/Headers/Include.h"
#include "Components/ActorComponent.h"
#include "../InkGuardGameMode.h"
#include "../Spawner/SpawnMgr.h"
#include "PlayerNetwork.generated.h"

class MyNetworkMgr;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INKGUARD_API UPlayerNetwork : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerNetwork();
	~UPlayerNetwork();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void SetPlayerStruct(FPlayerStruct tPlayerData);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	const FPlayerStruct& GetPlayerStruct();

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void SetPlayerHpForSendTransform(float fNewHP);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void AddAdditionalData(const FVaultingPacket tNewPacket); //�ݵ�� ���� ���� ����ؼ� �Է��Ұ�. <= my blueprint function library static �Լ�����.

#pragma region NetworkRoutine

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	bool CheckGameStart();

//============================================================= ���� ���� ��: �� //  ��: �Ʒ�

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void SendPlayerTransform(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void RecvPlayerTransform(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void SendPlayerInputData(float DeltaTime, const FPlayerInputStruct& tBakuInputData);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void RecvPlayerInputData(float DeltaTime, FPlayerInputStruct& tOutInputs);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void SendAdditionalData(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void RecvAdditionalData(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	const FVaultingPacket GetVaultingData();

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void TidyNetworkTickRoutine(); // ƽ���� �������� ��ƾ �ֱ�.

#pragma endregion

private:

private:
	MyNetworkMgr* m_pNetworkMgr = nullptr;

	FPlayerStruct m_tPlayerStruct; 
	float m_fSyncTimer = 0.f;
	float m_fBakuHealth = 100.f;
};
