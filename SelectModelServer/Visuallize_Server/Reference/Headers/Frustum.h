#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Tick();

public:
	void TransformToLocalSpace(_fmatrix WorldMatrixInv);
	_bool isInWorldSpace(_fvector vWorldPos, _float fRadius = 0.f);
	_bool isInLocalSpace(_fvector vLocalPos, _float fRadius = 0.f);

private:
	_float3			m_vOriginalPoint[8];
	_float3			m_vWorldPoint[8];
	_float4			m_WorldPlane[6];
	_float4			m_LocalPlane[6];

public:
	virtual void Free() override;
};

END