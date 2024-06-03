#include "..\Public\LevelManager.h"
#include "Level.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevelManager)

CLevelManager::CLevelManager()
	: m_pReservationLevel(nullptr)
	, m_bOpenReady(false)
	, m_iReservationLevelIndex(0)
{
}


HRESULT CLevelManager::OpenLevel(_uint iLevelIndex, CLevel* pNewLevel)
{
	m_pReservationLevel = pNewLevel;
	m_iReservationLevelIndex = iLevelIndex;

	return S_OK;
}

HRESULT CLevelManager::SceneSwap() {
	if (m_pCurrentLevel) {
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		//CLightManager* pLightManager = CLightManager::GetInstance();

		pGameInstance->Clear(m_iCurrentLevelIndex);
		//pLightManager->Free();
	}

	if (FAILED(m_pReservationLevel->Initialize())) {
		MSG_BOX(TEXT("Failed To Level Open"));
		SafeRelease(m_pReservationLevel);

		return E_FAIL;
	}

	SafeRelease(m_pCurrentLevel);

	if (m_pReservationLevel) {
		m_pCurrentLevel = m_pReservationLevel;

		m_iCurrentLevelIndex = m_iReservationLevelIndex;
	}

	m_pReservationLevel = nullptr;
	m_iReservationLevelIndex = 0;

	return S_OK;
}

CLevel* CLevelManager::GetLevel() {
	return m_pCurrentLevel;
}

void CLevelManager::Tick(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(fTimeDelta);
}

void CLevelManager::LateTick(_float fTimeDelta)
{
	if (m_pReservationLevel && m_bOpenReady) {
		SceneSwap();
	}

	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->LateTick(fTimeDelta);
}

HRESULT CLevelManager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

void CLevelManager::Free()
{
	SafeRelease(m_pCurrentLevel);
}
