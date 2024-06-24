#include "stdafx.h"
#include "..\Public\LevelLoading.h"
#include "GameInstance.h"

/* Levels */
#include "LevelLogo.h"
#include "LevelGamePlay.h"

#include "Loader.h"


CLevelLoading::CLevelLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
	, m_pLoader(nullptr)
	, m_pNextLevel(nullptr)
{
}

HRESULT CLevelLoading::Initialize(LEVEL eNextLevel)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(CreateIDToLevel(eNextLevel)))
		return E_FAIL;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel, m_pNextLevel);

	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevelLoading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (!m_pNextLevel) {
		return;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_pNextLevel->GetLoadFinished()) {
		pGameInstance->OpenLevel(m_eNextLevel, m_pNextLevel);

		m_pNextLevel = nullptr;
	}
}

HRESULT CLevelLoading::CreateIDToLevel(LEVEL eNextLevel) {
	m_eNextLevel = eNextLevel;

	switch (eNextLevel)
	{
	case LEVEL_LOGO:
		m_pNextLevel = CLevelLogo::Create(m_pDevice, m_pContext);
		break;

	case LEVEL_GAMEPLAY:
		m_pNextLevel = CLevelGamePlay::Create(m_pDevice, m_pContext);
		break;

	default:
		break;
	}

	if (!m_pNextLevel)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevelLoading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (m_pNextLevel)
		SetWindowText(g_hWnd, m_pNextLevel->GetLoadingText());

	return S_OK;
}

CLevelLoading* CLevelLoading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevelLoading*		pInstance = new CLevelLoading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoading"));
		SafeRelease(pInstance);
	}

	return pInstance;

}


void CLevelLoading::Free()
{
	__super::Free();

	SafeRelease(m_pLoader);

	if (m_pNextLevel)
		SafeRelease(m_pNextLevel);
}


