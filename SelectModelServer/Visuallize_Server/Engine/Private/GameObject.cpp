#include "..\Public\GameObject.h"
#include "GameInstance.h"
#include "Component.h"


CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_bDestroyed(false)
	, m_bVisible(true)
	, m_bActive(true)
	, m_iCurrentLevel(0)
{
	SafeAddRef(m_pDevice); 
	SafeAddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_bDestroyed(false)
	, m_bVisible(true)
	, m_bActive(true)
	, m_iCurrentLevel(rhs.m_iCurrentLevel)
{
	SafeAddRef(m_pDevice); 
	SafeAddRef(m_pContext);
}

CComponent* CGameObject::GetComponentPtr(const _tchar* pComponentTag)
{
	CComponent*	pComponent = FindComponent(pComponentTag);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;	
}

HRESULT CGameObject::InitializePrototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{
	return S_OK;
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::LateTick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render(_uint eRenderGroup)
{
	return S_OK;
}

HRESULT CGameObject::RenderLightDepth(CLight* pLight)
{
	return S_OK;
}

HRESULT CGameObject::AddComponent(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != FindComponent(pComponentTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	SafeAddRef(pGameInstance);
	
	CComponent*			pComponent = pGameInstance->CloneComponent(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	SafeAddRef(pComponent);

	SafeRelease(pGameInstance);	

	return S_OK;
}

HRESULT CGameObject::RemoveComponent(const _tchar* pComponentTag) 
{
	auto iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));

	if (iter == m_Components.end())
		return E_FAIL;

	CComponent* com = iter->second;
	SafeRelease(com);

	delete[] iter->first;

	m_Components.erase(iter);

	return S_OK;
}

HRESULT CGameObject::ComputeCamZ(_fvector vWorldPos)
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_fvector		vCamPos = XMLoadFloat4(&pPipeLine->GetCamPosition());

	m_fCamDistance = XMVectorGetX(XMVector3Length(vWorldPos - vCamPos));

	RELEASE_INSTANCE(CPipeLine);

	return S_OK;

}


CComponent * CGameObject::FindComponent(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		SafeRelease(Pair.second);

	m_Components.clear();

	SafeRelease(m_pDevice);
	SafeRelease(m_pContext);
}
