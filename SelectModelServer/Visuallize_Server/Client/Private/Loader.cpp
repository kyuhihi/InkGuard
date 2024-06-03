#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Level.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pNextLevel(nullptr)
{
	SafeAddRef(m_pDevice);
	SafeAddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->GetCS());

	pLoader->LoadingLevel();

	LeaveCriticalSection(&pLoader->GetCS());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID, CLevel* pNextLevel)
{
	m_eNextLevelID = eNextLevelID;

	m_pNextLevel = pNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::LoadingLevel()
{
	if (!m_pNextLevel)
		return E_FAIL;

	//BaseSetting(m_pNextLevel);

	m_pNextLevel->Loading(m_eNextLevelID);

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID, CLevel* pNextLevel){
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID, pNextLevel)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoader"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hThread);

	SafeRelease(m_pContext);
	SafeRelease(m_pDevice);
}
