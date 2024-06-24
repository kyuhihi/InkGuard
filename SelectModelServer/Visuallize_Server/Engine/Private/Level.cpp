#include "..\Public\Level.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_isLoadFinished(false)
	, m_eLevelID(0)
{
	SafeAddRef(m_pDevice);
	SafeAddRef(m_pContext);
}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

void CLevel::Tick(_float fTimeDelta)
{ 
}

void CLevel::LateTick(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

HRESULT CLevel::Loading(_int eLevelID)
{
	m_eLevelID = eLevelID;
	m_isLoadFinished = true;

	return S_OK;
}

void CLevel::Free()
{
	SafeRelease(m_pDevice); 
	SafeRelease(m_pContext);
}
