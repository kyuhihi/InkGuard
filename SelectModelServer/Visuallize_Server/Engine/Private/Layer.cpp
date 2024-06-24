#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

CComponent * CLayer::GetComponentPtr(const _tchar * pComponentTag, _uint iLayerIndex)
{
	if (iLayerIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (_uint i = 0; i < iLayerIndex; ++i)
		++iter;

	return (*iter)->GetComponentPtr(pComponentTag);	
}

HRESULT CLayer::AddGameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Initialize()
{

	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	auto iter = m_GameObjects.begin();

	while (iter != m_GameObjects.end())
	{
		if (nullptr != (*iter)) {
			if ((*iter)->GetDestroyed()) {
				CGameObject* ReleaseObject = (*iter);
				iter = m_GameObjects.erase(iter);

				SafeRelease(ReleaseObject);
			}
			else if ((*iter)->GetActive()) {
				(*iter)->Tick(fTimeDelta);
				++iter;
			}
			else {
				++iter;
			}
		}
	}
}

void CLayer::LateTick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && pGameObject->GetActive())
			pGameObject->LateTick(fTimeDelta);
	}
}

CLayer * CLayer::Create()
{
	CLayer*			pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLayer"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		SafeRelease(pGameObject);

	m_GameObjects.clear();
}


