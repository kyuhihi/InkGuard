#pragma once
#include "Component.h"

BEGIN(Engine)
class CRigidBody;
class ENGINE_DLL CConstantForce final : public CComponent
{
public:
	using FORCEDESC = struct tagForceDesc
	{
		PxRigidDynamic* pRigidBody = nullptr;
		_float3 vForce = _float3(0.f, 0.f, 0.f);
		// _float3 vRelativeForce;
		// _float3 vTorque;
		// _float3 vRelativeTorque;
	};


private:
	CConstantForce(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CConstantForce(const CConstantForce& rhs);
	virtual ~CConstantForce() = default;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta);

public:
	void DisableForce() { m_bForceActive = false; }
	void ActiveForce() { m_bForceActive = true; }
	const _bool IsActiveForce() const { return m_bForceActive; }
	void SetActiveForce(_bool bActive) { m_bForceActive = bActive; }

	void SetForce(_float3 vForce) { m_forceDesc.vForce = vForce; }
	const _float3 GetForce() const { return m_forceDesc.vForce; }
	void SetRigidBody(PxRigidDynamic* pRigidBody) { m_forceDesc.pRigidBody = pRigidBody; }

private:
	FORCEDESC m_forceDesc;
	_bool m_bForceActive;

public:
	static CConstantForce* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END