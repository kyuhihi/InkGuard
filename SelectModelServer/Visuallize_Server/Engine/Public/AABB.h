#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CAABB final : public CCollider
{
protected:
	CAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAABB(const CAABB& rhs);
	virtual ~CAABB() = default;

public:
	BoundingBox GetCollider() {
		return *m_pAABB;
	}
	

public:
	virtual HRESULT InitializePrototype(CCollider::TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix);
	virtual _bool Collision(CCollider* pTargetCollider);

public:
	_bool CollisionAABB(CCollider* pTargetCollider);

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif // _DEBUG

private:
	BoundingBox*			m_pOriginalAABB = nullptr;
	BoundingBox*			m_pAABB = nullptr;

private:
	_float3 ComputeMin();
	_float3 ComputeMax();


public:
	static CAABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCollider::TYPE eColliderType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;


};

END