#include "stdafx.h"
#include "..\Public\Glass.h"
#include "GameInstance.h"

CGlass::CGlass(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CGlass::CGlass(const CGlass& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGlass::InitializePrototype()
{
	return S_OK;
}

HRESULT CGlass::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	_float3 fInitPos = *(_float3*)pArg;
	

	return S_OK;
}

void CGlass::Tick(_float fTimeDelta)
{
	/*if (CGameInstance::GetInstance()->isInFrustumWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION)))
		m_bCulling = false;
	else {
		m_bCulling = true;
		return;
	}*/
		
	
	

}

void CGlass::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;
	
	_vector vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->GetCamPosition());
	_vector vMyPos= m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_vector vBillBoardLook = XMVector3Normalize(vMyPos - vCamPos);
	m_pTransformCom->LookAt(XMVectorSetW(m_pTransformCom->GetState(CTransform::STATE_POSITION) + vBillBoardLook,1.f));

	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CGlass::Render(_uint eRenderGroup)
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->SetRawValue("g_WorldMatrix", &m_pTransformCom->GetWorldFloat4x4TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_ViewMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_ProjMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->SetSRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0, 2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}



HRESULT CGlass::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Texture_InnerEmptyCircle"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CGlass* CGlass::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGlass*		pInstance = new CGlass(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CGlass"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CGlass::Clone(void * pArg)
{
	CGlass*		pInstance = new CGlass(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CGlass"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CGlass::Free()
{
	__super::Free();

	SafeRelease(m_pTextureCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
}
