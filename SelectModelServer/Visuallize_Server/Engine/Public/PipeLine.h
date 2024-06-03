#pragma once

#include "Base.h"

BEGIN(Engine)

class CPipeLine	final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)
public:
	enum TRANSFORMSTATE { 
		D3DTS_VIEW, 
		D3DTS_PROJ, 
		D3DTS_END 
	};

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void SetTransform(TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);

	_matrix GetTransformMatrix(TRANSFORMSTATE eTransformState) const {
		return XMLoadFloat4x4(&m_TransformMatrix[eTransformState]);
	}
	_float4x4 GetTransformFloat4x4(TRANSFORMSTATE eTransformState) const {
		return m_TransformMatrix[eTransformState];
	}
	_float4x4 GetTransformFloat4x4TP(TRANSFORMSTATE eTransformState) const {
		_float4x4 Transform;
		XMStoreFloat4x4(&Transform, XMMatrixTranspose(GetTransformMatrix(eTransformState)));
		return Transform;
	}
	_matrix GetTransformMatrixInverse(TRANSFORMSTATE eTransformState) const {
		return XMLoadFloat4x4(&m_TransformInverseMatrix[eTransformState]);
	}

	_float4 GetCamPosition() const {
		return m_vCamPosition;
	}

	void SetProjectionInfo(_float fNear, _float fFar) {
		m_fProjNear = fNear;
		m_fProjFar = fFar;
	}

	const _float GetProjectionFar() const {
		return m_fProjFar;
	}

	const _float GetProjectionNear() const {
		return m_fProjNear;
	}

public:
	void Update();
	
private:
	_float4x4 m_TransformMatrix[D3DTS_END];
	_float4x4 m_TransformInverseMatrix[D3DTS_END];
	_float4 m_vCamPosition;
	_float m_fProjNear;
	_float m_fProjFar;

public:
	virtual void Free() override;
};

END