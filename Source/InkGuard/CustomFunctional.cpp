// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomFunctional.h"

const XMFLOAT3 UCustomFunctional::FVector_To_float3(const FVector& vSource)
{
	return XMFLOAT3(vSource.X, vSource.Y, vSource.Z);
}

const FVector UCustomFunctional::float3_To_FVector(const _float3& vSource)
{
	return FVector(vSource.x, vSource.y, vSource.z);
}
