#pragma once

#include "Collider.h"

BEGIN(Engine)
class ENGINE_DLL CRigidBody final : public CCollider
{
public:
	using RIGIDBODYDESC = struct tagRigidBodyDesc
	{
		COLLIDERDESC colliderDesc;
		_float fMass = 10.f;
	};
private:
	using CAPSULEDESC = struct {
		_float4x4 WorldMatrix;
		_float3 vTopCenter;
		_float3 vBottomCenter;
		_float fHalfHeight;
	};
protected:
	CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() = default;

public:
	virtual HRESULT InitializePrototype(CCollider::TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix);
	virtual _bool Collision(CCollider* pTargetCollider);
	virtual void AttachShape() override;
	virtual void SetSize(_float3 vSize);
	void Tick(_float fTimeDelta);

public:
	PxRigidDynamic* GetPxRigidBody() { return m_pRigidBody; }

	void SetStepClimbHeight(_float fHeight) { m_fStepClimbHeight = fHeight; }
	void SetStepClimbSmooth(_float fSmooth) { m_fStepClimbSmooth = fSmooth; }

public:
	virtual PxTransform GetTransform() override;
	PxTransform GetRigidTransform();
	virtual void SyncTransform(class CTransform* pTransform); // rigid body의 위치와 회전을 transform에 적용시킵니다.
	void TestFunc(class CTransform* pTransform);
	void SyncNavigation(class CNavigation* pNavigation); // 네비게이션 처리

	void StepClimb(CTransform* pTransform, _float fTimeDelta);
	void StepWall(CTransform* pTransform, _float fTimeDelta);

	_bool IsGround(CTransform* pTransform);

	// 경사면
	_bool IsSlope(CTransform* pTransform);
	_vector GetSlopeLook(CTransform* pTransform); 


	void SetGroundLayerTag(const _tchar* pTag);
	void SetGroundColliderGroup(_uint iGroup);

	virtual void PxSleep() { if (m_pRigidBody) m_pRigidBody->putToSleep(); };
	virtual void PxWakeUp() { if (m_pRigidBody) m_pRigidBody->wakeUp(); };


public:
	const _float3 GetColliderCenter() const
	{
		return m_ColliderDesc.vCenter;
	}

#ifdef _DEBUG
public:
	virtual HRESULT Render(ID3D11DeviceContext* pContext) override;
#endif // _DEBUG

private:
	void CreateBody();

private:
	BoundingOrientedBox* m_pOriginalOBB = nullptr;
	BoundingOrientedBox* m_pOBB = nullptr;

	BoundingSphere* m_pTopSphere = nullptr;
	BoundingSphere* m_pTopOriginalSphere = nullptr;

	BoundingSphere* m_pBottomSphere = nullptr;
	BoundingSphere* m_pBottomOriginalSphere = nullptr;

	PxController* m_pController = nullptr;

	PxRigidDynamic* m_pRigidBody;
	RIGIDBODYDESC m_rigidDesc;
	PxRigidBody* TEst;

	CAPSULEDESC m_CapsuleDesc;

private:
	PxQueryFilterData m_groundFilterData;
	_float m_fStepClimbHeight;
	_float m_fStepClimbSmooth;
	_float m_fMaxSlopAngle;
	_float3 m_fPrevHitNormal;

public:
	static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCollider::TYPE eColliderType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

};
END