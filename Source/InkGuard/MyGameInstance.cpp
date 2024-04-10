// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "MyNetWorking/MyNetworkMgr.h"


void UMyGameInstance::Init()
{
	Super::Init();
}

void UMyGameInstance::Shutdown()
{
	Super::Shutdown();
	MyNetworkMgr::GetInstance()->DestroyInstance();

	m_pNetWorkMgr = nullptr;
}
