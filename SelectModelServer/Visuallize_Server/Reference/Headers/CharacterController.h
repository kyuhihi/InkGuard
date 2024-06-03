#pragma once

#include "Collider.h"
#include "RigidBody.h"

BEGIN(Engine)
class ENGINE_DLL CCharacterController final : public CCollider
{
protected:
	CCharacterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacterController(const CCharacterController& rhs);
	virtual ~CCharacterController() = default;

public:
	virtual HRESULT InitializePrototype(CCollider::TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix);
	virtual _bool Collision(CCollider* pTargetCollider);
	virtual void SetSize(_float3 vSize);
	virtual void AttachShape() override;

	void Tick(_float fTimeDelta);

public:
	PxRigidDynamic* GetPxRigidBody() { return m_pRigidBody; }
	PxController* GetPxController() { return m_pController; }
	void SetGround(_bool bGround) { m_bGround = bGround; }

public:
	virtual PxTransform GetTransform() override;
	PxTransform GetRigidTransform();
	virtual void SyncTransform(class CTransform* pTransform); // rigid body의 위치와 회전을 transform에 적용시킵니다.
	void TestFunc(class CTransform* pTransform);
	void SyncNavigation(class CNavigation* pNavigation); // 네비게이션 처리

#ifdef _DEBUG
public:
	virtual HRESULT Render(ID3D11DeviceContext* pContext) override;
#endif // _DEBUG

private:
	BoundingOrientedBox* m_pOriginalOBB = nullptr;
	BoundingOrientedBox* m_pOBB = nullptr;

	BoundingSphere* m_pTopSphere = nullptr;
	BoundingSphere* m_pTopOriginalSphere = nullptr;

	BoundingSphere* m_pBottomSphere = nullptr;
	BoundingSphere* m_pBottomOriginalSphere = nullptr;

	PxController* m_pController = nullptr;

	PxRigidDynamic* m_pRigidBody;
	CRigidBody::RIGIDBODYDESC m_rigidDesc;
	PxRigidBody* TEst;

private:
	_float m_fGravityPower;
	_float m_fGravity;
	_bool m_bGround;

public:
	static CCharacterController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCollider::TYPE eColliderType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};
END