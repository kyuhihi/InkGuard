#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL COBB final : public CCollider
{
public:
	using OBBDESC = struct tagOBBDesc
	{
		_float3 vCenter;
		_float3 vCenterAxis[3];
		_float3 vAlignAxis[3];
	};
protected:
	COBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COBB(const COBB& rhs);
	virtual ~COBB() = default;

public:
	BoundingOrientedBox GetCollider() {
		return *m_pOBB;
	}

public:
	virtual HRESULT InitializePrototype(CCollider::TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix);
	virtual _bool Collision(CCollider* pTargetCollider);

public:
	_bool CollisionOBB(CCollider* pTargetCollider);

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif // _DEBUG

private:
	BoundingOrientedBox* m_pOBB = nullptr;
	BoundingOrientedBox* m_pOriginalOBB = nullptr;

private:
	OBBDESC ComputeOBBDesc();

public:
	static COBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCollider::TYPE eColliderType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;


};

END