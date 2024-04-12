// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierSpawner.h"

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

