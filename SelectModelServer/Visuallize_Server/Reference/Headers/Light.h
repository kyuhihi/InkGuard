#pragma once

#include "Base.h"

BEGIN(Engine)
/* 빛 정볼르 보관한다. */

class CLight final : public CBase
{
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() = default;

public:
	HRESULT Initialize(const LIGHTDESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBufferRect* pVIBuffer);

public:
	LIGHTDESC* GetLightDesc() {
		return &m_lightDesc;
	}

	const _float4x4 GetViewMatrixTP() const {
		_vector vLightPosition = XMLoadFloat4(&m_lightDesc.vPosition);
		_matrix viewMatrix = XMMatrixLookAtLH(vLightPosition,
			XMLoadFloat4(&m_lightDesc.vDirection),
			XMVectorSet(0.f, 1.f, 0.f, 1.f));

		_float4x4 returnMatrix;
		XMStoreFloat4x4(&returnMatrix, XMMatrixTranspose(viewMatrix));
		return returnMatrix;
	}

	const _float4x4 GetProjectionMatrixTP() const {
		_matrix projMatrix = XMMatrixPerspectiveFovLH(static_cast<_float>(XM_PI / 2.f), 1.f, 0.2f, 300.f);
		
		_float4x4 returnMatrix;
		XMStoreFloat4x4(&returnMatrix, XMMatrixTranspose(projMatrix));
		return returnMatrix;
	}

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

private:
	LIGHTDESC m_lightDesc;

public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	virtual void Free() override;
};

END