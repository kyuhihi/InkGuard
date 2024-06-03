#include "stdafx.h"
#include "..\Public\ShaderCam.h"
#include "GameInstance.h"

CShaderCam::CShaderCam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CShaderCam::CShaderCam(const CShaderCam & rhs, CTransform::TRANSFORMDESC * pArg)
	: CCamera(rhs, pArg)
{

}

HRESULT CShaderCam::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShaderCam::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyShaderComponents()))
		return E_FAIL;

	return S_OK;
}

void CShaderCam::Tick(_float fTimeDelta)
{
	KeyInput(fTimeDelta);

	_float4 fCamDirection = { 0.f, 0.f, 0.f, 0.f };

	XMStoreFloat4(&fCamDirection, m_pTransformCom->GetState(CTransform::STATE_POSITION));

	m_vShaderCom[SHADERCAM::SHADER_RIM_ANIM]->SetRawValue("g_CameraDirection", &fCamDirection, sizeof(_float4));

	__super::Tick(fTimeDelta);
}

void CShaderCam::LateTick(_float fTimeDelta)
{
}

HRESULT CShaderCam::Render()
{
	return S_OK;
}

HRESULT CShaderCam::ReadyShaderComponents()
{
	CShader* pShaderCom = nullptr;

	/* For.Com_Shader_LimLighting_Anim */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Shader_RimLighting_Anim"), TEXT("Com_Shader_Rim_Anim"), (CComponent**)&pShaderCom)))
		return E_FAIL;

	m_vShaderCom.push_back(pShaderCom);

	return S_OK;
}

void CShaderCam::KeyInput(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Move_Cam */
	if (pGameInstance->GetDIKState(DIK_W) & 0x80)
		m_pTransformCom->GoStraight(fTimeDelta);

	if (pGameInstance->GetDIKState(DIK_S) & 0x80)
		m_pTransformCom->GoBackward(fTimeDelta);

	if (pGameInstance->GetDIKState(DIK_A) & 0x80)
		m_pTransformCom->GoLeft(fTimeDelta);

	if (pGameInstance->GetDIKState(DIK_D) & 0x80)
		m_pTransformCom->GoRight(fTimeDelta);

	/* For.Turn_Cam_View */
	if (pGameInstance->GetDIMKeyState(DIMK_RBUTTON)) {
		_long	MouseMove = 0;

		if (MouseMove = pGameInstance->GetDIMMoveState(DIMM_X))
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.08f);

		if (MouseMove = pGameInstance->GetDIMMoveState(DIMM_Y))
			m_pTransformCom->Turn(m_pTransformCom->GetState(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.08f);
	}

	/*CTransform*	pTargetTransform = (CTransform*)pGameInstance->GetComponentPtr(LEVEL::LEVEL_SHADERTOOL, TEXT("Layer_ShaderAnim"), TEXT("Com_Transform"), 0);

	_long	MouseMove = 0;
	if (MouseMove = pGameInstance->GetDIMMoveState(DIMM_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.08f);
		pTargetTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.08f);
	}
	if (MouseMove = pGameInstance->GetDIMMoveState(DIMM_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->GetState(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.08f);
	}

	_long	WheelMove = 0;
	if (WheelMove = pGameInstance->GetDIMMoveState(DIMM_WHEEL)) {
		m_fCTP -= (_float)WheelMove * 0.005f;
	}

	_vector vMyLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);
	_vector vMyPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_vector vTargetPos = pTargetTransform->GetState(CTransform::STATE_POSITION);
	vTargetPos.m128_f32[1] += 0.6f;

	_vector vMovePos = vTargetPos - (vMyPos + (vMyLook * m_fCTP));

	m_pTransformCom->SetState(CTransform::STATE_POSITION, vMyPos + vMovePos);*/

	RELEASE_INSTANCE(CGameInstance);
}

CShaderCam * CShaderCam::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShaderCam* pInstance = new CShaderCam(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCameraFree"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CShaderCam::Clone(void* pArg)
{
	CCamera::CAMERADESC*		pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CShaderCam* pInstance = new CShaderCam(*this, &pCameraDesc->TransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTerrain"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CShaderCam::Free()
{
	__super::Free();

	for (auto& pShaderCom : m_vShaderCom)
		SafeRelease(pShaderCom);

	m_vShaderCom.clear();
}
