// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../MyNetWorking/Headers/Include.h"
#include "CustomFunctional.generated.h"

/**
 * 
 */
UCLASS()
class INKGUARD_API UCustomFunctional : public UObject
{
	GENERATED_BODY()

public:
	static const XMFLOAT3 FVector_To_float3(const FVector& vSource);
	static const FVector float3_To_FVector(const _float3& vSource);
	
};
