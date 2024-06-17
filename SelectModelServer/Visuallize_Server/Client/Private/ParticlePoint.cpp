#include "stdafx.h"
#include "..\Public\ParticlePoint.h"
#include "GameInstance.h"

CParticlePoint::CParticlePoint(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CParticlePoint::CParticlePoint(const CParticlePoint & rhs)
	: CGameObject(rhs)
{

}

HRESULT CParticlePoint::InitializePrototype()
{
	return S_OK;
}

HRESULT CParticlePoint::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CParticlePoint::Tick(_float fTimeDelta)
{
	m_pVIBufferCom->Update(fTimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_pTargetTransform == nullptr) {
		m_pTargetTransform = (CTransform*)pGameInstance->GetComponentPtr(m_iCurrentLevel, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	}

	_vector vTargetPos = m_pTargetTransform->GetState(CTransform::STATE_POSITION);
	_vector vTargetLook = XMVector4Normalize(m_pTargetTransform->GetState(CTransform::STATE_LOOK));
	vTargetPos -= vTargetLook * 1.1f; //플레이어 뒤로.
	m_pTransformCom->SetState(CTransform::STATE_POSITION, vTargetPos);
	

	RELEASE_INSTANCE(CGameInstance);

}

void CParticlePoint::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	if (CGameInstance::GetInstance()->GetDIKState(DIK_LSHIFT) & 0x80)
		m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CParticlePoint::Render(_uint eRenderGroup)
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0, 0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	return S_OK;
}

HRESULT CParticlePoint::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);

	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Shader_PointInstance"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_VIBuffer_Point_Instance"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Texture_Snow"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;	

	return S_OK;
}

HRESULT CParticlePoint::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->SetRawValue("g_WorldMatrix", &m_pTransformCom->GetWorldFloat4x4TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_ViewMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_ProjMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_vCamPosition", &pGameInstance->GetCamPosition(), sizeof(_float4))))
		return E_FAIL;
	
	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->SetSRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;	

	return S_OK;
}


CParticlePoint * CParticlePoint::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticlePoint*		pInstance = new CParticlePoint(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CParticlePoint"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CParticlePoint::Clone(void * pArg)
{
	CParticlePoint*		pInstance = new CParticlePoint(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CParticlePoint"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CParticlePoint::Free()
{
	__super::Free();

	SafeRelease(m_pTextureCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);

	m_pTargetTransform = nullptr;
}
