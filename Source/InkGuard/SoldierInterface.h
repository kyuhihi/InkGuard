// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
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
};
