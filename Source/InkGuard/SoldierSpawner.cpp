// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierSpawner.h"
#include "SpawnMgr.h"


// Sets default values
ASoldierSpawner::ASoldierSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASoldierSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASoldierSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASoldierSpawner::AppendNewDuty(const int& iSoldierType)
{
	FSpawnDutyStruct NewDuty;
	NewDuty.iSpawnType = iSoldierType;

	m_tSpawnInfos.Add(NewDuty);
}

void ASoldierSpawner::InitializeAboutSpawnerBySoldierInterface(AActor* pActor)
{
	ISoldierInterface* pTargetInterface = nullptr; 
	pTargetInterface  = Cast<ISoldierInterface>(pActor);
	if (pTargetInterface)
	{
		pTargetInterface->SetAboutColor(iSpawnerColor);
		pTargetInterface->SetAboutSpawner(this);
	}
}

