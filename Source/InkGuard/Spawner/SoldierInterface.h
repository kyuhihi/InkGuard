// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Customs/CustomStructs.h"
#include "SoldierInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class USoldierInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INKGUARD_API ISoldierInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawner")
	void SetAboutColor(const int NewColor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawner")
	void SetAboutSpawner(AActor* pOwnerSpawner);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawner")
	const bool IsOurTeam();


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawner")
	const bool IsDead();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawner")
	void Respawn();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawner")
	void SetValueOfPacket(FPACKET_SOLDIER_TRANSFORM_BLUEPRINT tRecvPacket);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawner")
	float GetHP();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawner")
	int GetLastDamageCauser();


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawner")
	const bool GetEffectPoint(FVector MyLocation, FTransform& vEffectPoint);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawner")
	void SpawnerTick(float fTimeDelta);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawner")
	int GetSoldierTypeForCpp();

};
