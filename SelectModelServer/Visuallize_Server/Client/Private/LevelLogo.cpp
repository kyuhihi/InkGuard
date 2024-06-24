#include "stdafx.h"
#include "..\Public\LevelLogo.h"

/* loading resource */
#include "BackGround.h"

#include "GameInstance.h"
#include "LevelLoading.h"

CLevelLogo::CLevelLogo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevelLogo::Loading(_int eLevelID)
{
	lstrcpy(m_szLoadingText, TEXT("��ü������ �ε����Դϴ�. "));

	//CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	//SafeAddRef(pGameInstance);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* ������ü���� �ε��Ѵ�. */

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�. "));
	/* �ؽ��ĸ� �ε��Ѵ�. */

	/* For.Prototype_Component_Texture_Default */
	if (FAILED(pGameInstance->AddPrototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("���� �ε����Դϴ�. "));
	/* �𵨸� �ε��Ѵ�. */


	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����Ϥ���.  "));

	RELEASE_INSTANCE(CGameInstance);

	return __super::Loading(eLevelID);
}

HRESULT CLevelLogo::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;


	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevelLogo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		SafeAddRef(pGameInstance);
				
		if (FAILED(pGameInstance->OpenLevel(LEVEL_LOADING, CLevelLoading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;



		SafeRelease(pGameInstance);

	}
}

HRESULT CLevelLogo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("�ΰ�����"));

	return S_OK;
}

HRESULT CLevelLogo::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	SafeAddRef(pGameInstance);

	if (!pGameInstance->AddGameObjectToLayer(TEXT("Prototype_GameObject_BackGround"), LEVEL_LOGO, pLayerTag))
		return E_FAIL;

	SafeRelease(pGameInstance);

	return S_OK;
}

CLevelLogo * CLevelLogo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevelLogo*		pInstance = new CLevelLogo(pDevice, pContext);

	return pInstance;
}

void CLevelLogo::Free()
{
	__super::Free();




}


