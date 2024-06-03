#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	using TRANSFORMDESC = struct tagTransformDesc
	{
		_float fSpeedPerSec;
		_float fRotationPerSec;
	};


private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector GetState(STATE eState) {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	}

	_matrix GetWorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_float4x4 GetWorldFloat4x4() const {
		return m_WorldMatrix;
	}

	_float4x4 GetWorldFloat4x4TP() const {
		_float4x4	WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(GetWorldMatrix()));
		return WorldMatrix;
	}

	_matrix GetWorldMatrixInverse() const {
		return XMMatrixInverse(nullptr, GetWorldMatrix());
	}

	void SetState(STATE eState, _fvector vState);

	/* For. Gizmo 오브젝트가 선택 되었을때 호출되는 함수. */
	void EditTransform(_bool editTransformDecomposition);

	void SetSpeedPerSec(_float fSpeed) {
		m_TransformDesc.fSpeedPerSec = fSpeed;
	}
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void GoStraight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void GoBackward(_float fTimeDelta);
	void GoLeft(_float fTimeDelta);
	void GoRight(_float fTimeDelta);

	void GoUp(_float fTimeDelta);
	void GoDown(_float fTimeDelta);

	void SetScale(_fvector vScaleInfo);
	_float3 GetScale();

	void Turn(_fvector vAxis, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);

	void LookAt(_fvector vAt);
	void LookAt_ForLandObject(_fvector vAt);
	void Move(_fvector vTargetPos, _float fTimeDelta, _float fLimitDistance = 0.1f);

	void MoveToSlowOut(_fvector vTargetPosition, _float fTimeDelta); // 지정한 위치로 다가가게 합니다 갈수록 속도가 느려집니다.


private:
	_float4x4 m_WorldMatrix;
	TRANSFORMDESC m_TransformDesc;

	/* For. ImGuizmo Option */
	ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END