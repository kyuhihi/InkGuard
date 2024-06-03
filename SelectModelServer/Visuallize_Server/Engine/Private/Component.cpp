#include "..\Public\Component.h"


CComponent::CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
	, m_isCloned(false)
{
	SafeAddRef(m_pDevice); 
	SafeAddRef(m_pContext);
}

CComponent::CComponent(const CComponent & rhs)
	: m_pDevice(rhs.m_pDevice), m_pContext(rhs.m_pContext)
	, m_isCloned(true)
{
	SafeAddRef(m_pDevice); 
	SafeAddRef(m_pContext);
}

HRESULT CComponent::InitializePrototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void * pArg)
{
	return S_OK;
}

void CComponent::Free()
{
SafeRelease(m_pDevice); 
SafeRelease(m_pContext);
}
