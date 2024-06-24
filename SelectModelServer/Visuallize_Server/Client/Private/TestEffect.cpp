#include "stdafx.h"
#include "../../Client/Public/TestEffect.h"
#include "GameInstance.h"

CTestEffect::CTestEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTestEffect::CTestEffect(const CTestEffect & rhs)
	: CGameObject(rhs),
	  m_eDesc(rhs.m_eDesc)
{
}

HRESULT CTestEffect::InitializePrototype()
{
	m_eDesc.strTextureName = nullptr;

	return S_OK;
}

HRESULT CTestEffect::Initialize(void * pArg)
{
	if(pArg != nullptr)
		memcpy(&m_eDesc, pArg, sizeof(EffectDesc));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTestEffect::Tick(_float fTimeDelta)
{
	m_fFrame += fTimeDelta;

	if (m_fFrame >= m_eDesc.fDuration)
	{
		m_fFrame = 0.f;
		m_bActive = false;
	}

	if (m_eDesc.eType == CUSTOM)
	{
		m_pTransformCom->SetState(CTransform::STATE_LOOK, XMVectorSet(m_eDesc.fDir.x, m_eDesc.fDir.y, m_eDesc.fDir.z, 0.f));
	}
	else if (m_eDesc.eType == CONVERAGE)
	{
		m_pTransformCom->SetState(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	}
	else
	{
		m_pTransformCom->SetState(CTransform::STATE_LOOK, XMVectorSet(m_eDesc.fDir.x, m_eDesc.fDir.y, m_eDesc.fDir.z, 0.f));
	}

	m_pTransformCom->GoStraight(fTimeDelta);

}

void CTestEffect::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	ComputeCamZ(m_pTransformCom->GetState(CTransform::STATE_POSITION));

	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CTestEffect::Render(_uint eRenderGroup)
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

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

HRESULT CTestEffect::Ready_Components()
{
	CTransform::TRANSFORMDESC desc;
	desc.fSpeedPerSec = m_eDesc.fSpeed;
	desc.fRotationPerSec = m_eDesc.fRotateSpeed;

	/* For.Com_Transform */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &desc)))
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
	if (FAILED(__super::AddComponent(m_iCurrentLevel, m_eDesc.strTextureName, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

CTestEffect * CTestEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTestEffect*		pInstance = new CTestEffect(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTestEffect"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CTestEffect::Clone(void * pArg)
{
	CTestEffect*		pInstance = new CTestEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTestEffect"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CTestEffect::Free()
{

	if (m_eDesc.strTextureName != nullptr)
	{
		delete[] m_eDesc.strTextureName;
		m_eDesc.strTextureName = nullptr;
	}

	__super::Free();

	SafeRelease(m_pTextureCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
}
