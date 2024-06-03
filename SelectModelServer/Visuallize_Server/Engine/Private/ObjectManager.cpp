#include "..\Public\ObjectManager.h"
#include "GameObject.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CObjectManager)

CObjectManager::CObjectManager()
{
}

CComponent * CObjectManager::GetComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iLayerIndex)
{
	CLayer*		pLayer = FindLayer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->GetComponentPtr(pComponentTag, iLayerIndex);
}

HRESULT CObjectManager::ReserveContainer(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return S_OK;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

CGameObject* CObjectManager::CloneGameObject(const _tchar* pPrototypeTag, void* pArg)
{
	CGameObject* pPrototype = FindPrototype(pPrototypeTag);

	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pGameObject = pPrototype->Clone(pArg);

	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

CGameObject* CObjectManager::CloneGameObject(const _tchar* pPrototypeTag, _uint iLevelIndex, void* pArg)
{
	CGameObject* pPrototype = FindPrototype(pPrototypeTag);

	if (nullptr == pPrototype)
		return nullptr;

	pPrototype->SetCurrentLevel(iLevelIndex); // 현재 레벨을 설정해줌

	CGameObject*	pGameObject = pPrototype->Clone(pArg);

	if (nullptr == pGameObject)
		return nullptr;


	return pGameObject;
	
}

HRESULT CObjectManager::AddPrototype(const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	auto iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTagFinder(pPrototypeTag));

	if (iter != m_Prototypes.end()) {
		SafeRelease(iter->second);
		m_Prototypes.erase(iter);
	}

	m_Prototypes.emplace(pPrototypeTag, pPrototype);			

	return S_OK;
}

CGameObject* CObjectManager::AddGameObjectToLayer(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg)
{
	CGameObject* pGameObject = CloneGameObject(pPrototypeTag, iLevelIndex, pArg);

	if (nullptr == pGameObject)
		return nullptr;

	CLayer*	pLayer = FindLayer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return nullptr;

		pLayer->AddGameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		pLayer->AddGameObject(pGameObject);
	}

	return pGameObject;
}

void CObjectManager::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
			{
				Pair.second->Tick(fTimeDelta);
			}
		}			
	}
	
	
}

void CObjectManager::LateTick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->LateTick(fTimeDelta);
		}
	}
}

void CObjectManager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
	{
		SafeRelease(Pair.second);
	}

	m_pLayers[iLevelIndex].clear();
}

CGameObject * CObjectManager::FindPrototype(const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTagFinder(pPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;	
}

CLayer * CObjectManager::FindLayer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;	
}

void CObjectManager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			SafeRelease(Pair.second);

		m_pLayers[i].clear();
	}

	SafeDeleteArray(m_pLayers);


	for (auto& Pair : m_Prototypes)	
		SafeRelease(Pair.second);

	m_Prototypes.clear();
	
}
