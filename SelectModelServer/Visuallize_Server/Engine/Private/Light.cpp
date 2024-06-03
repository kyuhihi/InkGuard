#include "..\Public\Light.h"
#include "Shader.h"
#include "VIBufferRect.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	SafeAddRef(m_pDevice);
	SafeAddRef(m_pContext);
}

HRESULT CLight::Initialize(const LIGHTDESC & LightDesc)
{
	m_lightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CShader * pShader, CVIBufferRect * pVIBuffer)
{
	_uint		iPassIndex = 0;

	if (LIGHTDESC::TYPE_DIRECTIONAL == m_lightDesc.eType)
	{
		iPassIndex = 1;

		if (FAILED(pShader->SetRawValue("g_vLightDir", &m_lightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(pShader->SetRawValue("g_vLightPos", &m_lightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->SetRawValue("g_fLightRange", &m_lightDesc.fRange, sizeof(_float))))
			return E_FAIL;

		iPassIndex = 2;
	}
	if (FAILED(pShader->SetRawValue("g_vLightDiffuse", &m_lightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->SetRawValue("g_vLightAmbient", &m_lightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->SetRawValue("g_vLightSpecular", &m_lightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(0, iPassIndex);

	pVIBuffer->Render();

	return S_OK;
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight*			pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX(TEXT("Failed To Created : CLight"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	SafeRelease(m_pDevice);
	SafeRelease(m_pContext);
}
