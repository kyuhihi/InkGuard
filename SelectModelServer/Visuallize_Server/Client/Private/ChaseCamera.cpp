#include "stdafx.h"
#include "..\Public\ChaseCamera.h"
#include "GameInstance.h"

CChaseCamera::CChaseCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
	, m_pChaseTransform(nullptr)
{
}

CChaseCamera::CChaseCamera(const CChaseCamera & rhs, CTransform::TRANSFORMDESC * pArg)
	: CCamera(rhs, pArg)
	, m_pChaseTransform(nullptr)
{

}

HRESULT CChaseCamera::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CChaseCamera::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CChaseCamera::Tick(_float fTimeDelta)
{
	//FreeModeTick();

	if (m_pChaseTransform) {
		_vector vLookPosition = m_pChaseTransform->GetState(CTransform::STATE_POSITION);
		_vector vChasePosition = vLookPosition;
		_float fLook = -5.f;
		_float fUp = 9.f;

		_matrix rotationMatrix = XMMatrixIdentity();

		rotationMatrix *= XMMatrixRotationX(XMConvertToRadians(-5)) *
			XMMatrixRotationY(XMConvertToRadians(0)) *
			XMMatrixRotationZ(XMConvertToRadians(0));

		vChasePosition = vChasePosition + XMVectorSet(0.f, fUp, fLook, 0.f);
		vChasePosition = XMVector3TransformCoord(vChasePosition, rotationMatrix);

		m_pTransformCom->MoveToSlowOut(vChasePosition, fTimeDelta);
		m_pTransformCom->LookAt(vLookPosition);
	}

	__super::Tick(fTimeDelta);
}

void CChaseCamera::LateTick(_float fTimeDelta)
{
}

HRESULT CChaseCamera::Render()
{
	return S_OK;
}

void CChaseCamera::FreeModeTick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

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
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.05f);
	}

	if (MouseMove = pGameInstance->GetDIMMoveState(DIMM_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->GetState(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);
	}
}

CChaseCamera* CChaseCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChaseCamera* pInstance = new CChaseCamera(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCameraFree"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CChaseCamera::Clone(void* pArg)
{
	CCamera::CAMERADESC*		pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CChaseCamera* pInstance = new CChaseCamera(*this, &pCameraDesc->TransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTerrain"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CChaseCamera::Free()
{
	__super::Free();
}
