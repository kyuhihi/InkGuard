// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"
#include "MyNetWorking/Headers/Include.h"

const FVaultingPacket UMyBlueprintFunctionLibrary::BuildVaultingPacket(FVector VaultingStart, FVector VaultingMiddle, FVector VaultingEnd)
{
	FVaultingPacket RetPacket;
	RetPacket.ePacketType = EAdditionalPacketType::ADD_VAULT;
	
	RetPacket.VaultingStart		= VaultingStart;
	RetPacket.VaultingMiddle	= VaultingMiddle;
	RetPacket.VaultingEnd		= VaultingEnd;


	return RetPacket;
}
