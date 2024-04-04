// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomStructs.h"
#include "MyNetWorking/Include.h"
#include "Components/ActorComponent.h"
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
	void TidyNetworkTickRoutine();

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	bool CheckGameStart();

#pragma region NetworkRoutine

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void SendPlayerTransform(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void RecvPlayerTransform(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void SendPlayerInputData(float DeltaTime, const FPlayerInputStruct& tBakuInputData);

	UFUNCTION(BlueprintCallable, Category = "MyNetworking")
	void RecvPlayerInputData(float DeltaTime, FPlayerInputStruct& tOutInputs);

#pragma endregion
private:


private:
	MyNetworkMgr* m_pNetworkMgr = nullptr;
	FPlayerStruct m_tPlayerStruct; //Player->True
	float m_fSyncTimer = 0.f;
	
	bool m_bGameStart = false;

	GAME_PLAY m_eGameTeam = GAME_PLAY::GAME_END;
};
