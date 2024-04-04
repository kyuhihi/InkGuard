// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CustomStructs.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INKGUARD_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "KeyPressingEvent")
	bool AboutKeyPressingF();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "KeyPressingEvent")
	const FPlayerInputStruct GetPlayerInputData();

};


