#include "..\Public\LightManager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLightManager)

CLightManager::CLightManager()
{
}

const LIGHTDESC * CLightManager::GetLightDesc(_uint iIndex)
{
	auto	iter = m_lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->GetLightDesc();	
}

HRESULT CLightManager::AddLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight* pLight = CLight::Create(pDevice, pContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_lights.push_back(pLight);

	return S_OK;
}

HRESULT CLightManager::Render(CShader * pShader, CVIBufferRect * pVIBuffer)
{
	for (auto& pLight : m_lights)
		pLight->Render(pShader, pVIBuffer);

	return S_OK;
}

void CLightManager::Free()
{
	for (auto& pLight : m_lights)
		SafeRelease(pLight);

	m_lights.clear();
}
