#include "..\Public\ComponentManager.h"
#include "Component.h"

IMPLEMENT_SINGLETON(CComponentManager)

CComponentManager::CComponentManager()
{

}

HRESULT CComponentManager::ReserveContainer(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return S_OK;

	m_iNumLevels = iNumLevels;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	return S_OK;
}

HRESULT CComponentManager::AddPrototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	if (nullptr != FindComponent(iLevelIndex, pPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CComponent * CComponentManager::CloneComponent(_uint iLevelIndex, const _tchar * pPrototypeTag, void* pArg)
{
	CComponent*		pPrototype = FindComponent(iLevelIndex, pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	return pPrototype->Clone(pArg);
}

CComponent * CComponentManager::FindComponent(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTagFinder(pPrototypeTag));

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;	

	return iter->second;
}

void CComponentManager::Clear(_uint iLevelIndex) {
	for (auto& iter : m_pPrototypes[iLevelIndex]) {
		SafeRelease(iter.second);
	}

	m_pPrototypes[iLevelIndex].clear();
}

void CComponentManager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])		
			SafeRelease(Pair.second);

		m_pPrototypes[i].clear();		
	}

	SafeDeleteArray(m_pPrototypes);
	
}
