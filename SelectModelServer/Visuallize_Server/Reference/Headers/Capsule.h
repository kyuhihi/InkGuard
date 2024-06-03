#pragma once
#include "RigidBody.h"
#include "Collider.h"

BEGIN(Engine)
class ENGINE_DLL CCapsule final : public CCollider
{//쓰지마세요 캡슐이긴함.
protected:
	CCapsule(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCapsule(const CCapsule& rhs);
	virtual ~CCapsule() = default;

public:
	virtual HRESULT InitializePrototype(CCollider::TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix);
	virtual _bool Collision(CCollider* pTargetCollider);
	virtual void AttachShape() override;
	virtual void SetSize(_float3 vSize);
	PxTransform GetTransform() override;

public:
	PxRigidDynamic* GetPxRigidBody() { return m_pRigidBody; }

public:
	PxTransform GetRigidTransform();
	PxBounds3	GetRigidBounds();
	void SyncTransform(class CTransform* pTransform); // rigid body의 위치와 회전을 transform에 적용시킵니다.
	void SyncNavigation(class CNavigation* pNavigation); // 네비게이션 처리

#ifdef _DEBUG
public:
	virtual HRESULT Render(ID3D11DeviceContext* pContext) override;
#endif // _DEBUG

private:
	BoundingSphere* m_pOriginalOBB = nullptr;
	BoundingSphere* m_pOBB = nullptr;

	PxRigidDynamic* m_pRigidBody;

	CRigidBody::RIGIDBODYDESC m_rigidDesc;

public:
	static CCapsule* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCollider::TYPE eColliderType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

};
END