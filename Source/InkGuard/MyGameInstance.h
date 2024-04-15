// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyNetWorking/Include.h"
#include "MyGameInstance.generated.h"

class MyNetworkMgr;

/**
 * 
 */
UCLASS()
class INKGUARD_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	/** virtual function to allow custom GameInstances an opportunity to set up what it needs */
	virtual void Init() override;

	/** virtual function to allow custom GameInstances an opportunity to do cleanup when shutting down */
	virtual void Shutdown() override;

private:
	MyNetworkMgr* m_pNetWorkMgr = nullptr;
};
