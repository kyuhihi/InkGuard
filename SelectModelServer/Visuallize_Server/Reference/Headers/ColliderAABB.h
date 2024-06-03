#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CColliderAABB final : public CCollider
{
protected:
	CColliderAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CColliderAABB(const CColliderAABB& rhs);
	virtual ~CColliderAABB() = default;

public:
	BoundingBox GetCollider() {
		return *m_pAABB;
	}
	
public:
	virtual HRESULT InitializePrototype(CCollider::TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix);
	virtual _bool Collision(CCollider* pTargetCollider);
	virtual void SetSize(_float3 vSize);
	virtual void AttachShape() override;

	virtual PxTransform GetTransform() override;

public:
	_bool CollisionAABB(CCollider* pTargetCollider);

#ifdef _DEBUG
public:
	virtual HRESULT Render(ID3D11DeviceContext* pContext) override;
#endif // _DEBUG

private:
	BoundingBox* m_pOriginalAABB = nullptr;
	BoundingBox* m_pAABB = nullptr;
	PxRigidStatic* m_pAABBBody;

private:
	_float3 ComputeMin();
	_float3 ComputeMax();


public:
	static CColliderAABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCollider::TYPE eColliderType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;


};

END