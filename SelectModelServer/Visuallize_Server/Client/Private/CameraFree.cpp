#include "stdafx.h"
#include "..\Public\CameraFree.h"
#include "GameInstance.h"

CCameraFree::CCameraFree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCameraFree::CCameraFree(const CCameraFree & rhs, CTransform::TRANSFORMDESC * pArg)
	: CCamera(rhs, pArg)
{

}

HRESULT CCameraFree::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraFree::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCameraFree::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	SafeAddRef(pGameInstance);

	if (!g_bKeyInput)
		return;

	if (pGameInstance->GetDIKState(DIK_R) & 0x80)
	{
		m_pPlayer->PlacePlayerZeroBase();
	}

	if (pGameInstance->GetDIKDownState(DIK_F2))
	{
		m_bFreeCam = !m_bFreeCam;
	}

	if (!m_bFreeCam) {
		if (m_pPlayer)//플레이어따라다님
		{
			m_pPlayer->KeyInput(m_pTransformCom, fTimeDelta);
		}
	}
	else
	{
		if (pGameInstance->GetDIKState(DIK_W) & 0x80)
		{
			m_pTransformCom->GoStraight(fTimeDelta);
		}

		if (pGameInstance->GetDIKState(DIK_S) & 0x80)
		{
			m_pTransformCom->GoBackward(fTimeDelta);
		}

		if (pGameInstance->GetDIKState(DIK_A) & 0x80)
		{
			m_pTransformCom->GoLeft(fTimeDelta);
		}

		if (pGameInstance->GetDIKState(DIK_D) & 0x80)
		{
			m_pTransformCom->GoRight(fTimeDelta);
		}
		_long	MouseMove = 0;

		if (MouseMove = pGameInstance->GetDIMMoveState(DIMM_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta*0.01f);
		}

		if (MouseMove = pGameInstance->GetDIMMoveState(DIMM_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->GetState(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.01f);
		}
	}
	

	SafeRelease(pGameInstance);

	__super::Tick(fTimeDelta);
}

void CCameraFree::LateTick(_float fTimeDelta)
{
}

HRESULT CCameraFree::Render()
{
	return S_OK;
}

CCameraFree * CCameraFree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCameraFree*		pInstance = new CCameraFree(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCameraFree"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CCameraFree::Clone(void* pArg)
{
	CCamera::CAMERADESC*		pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CCameraFree*		pInstance = new CCameraFree(*this, &pCameraDesc->TransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTerrain"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CCameraFree::Free()
{
	__super::Free();


}
