#include "stdafx.h"
#include "..\Public\Ground.h"
#include "GameInstance.h"
#include "Light.h"

CGround::CGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, GROUND_SIZE(70.F)
{
}

CGround::CGround(const CGround& rhs)
	: CGameObject(rhs)
	, GROUND_SIZE(rhs.GROUND_SIZE)
{
}

HRESULT CGround::InitializePrototype()
{
	return S_OK;
}

HRESULT CGround::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	//m_pTransformCom->SetState(CTransform::STATE_POSITION, XMVectorSet(static_cast<_float>(rand() % 10), 0.f, static_cast<_float>(rand() % 10), 1.f));
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f),XMConvertToRadians(90.f));
	m_pTransformCom->SetScale(XMVectorSet(GROUND_SIZE, GROUND_SIZE, 1.f, 1.f));

	return S_OK;
}

void CGround::Tick(_float fTimeDelta)
{
	//m_pTransformCom->EditTransform(true);
}

void CGround::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	//m_pRendererCom->AddRenderGroup(CRenderer::RENDER_SHADOW_DEPTH, this);
	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CGround::Render(_uint eRenderGroup)
{

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

	if (FAILED(m_pShaderCom->Begin(0, 0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CGround::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Terrain"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CGround* CGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGround*		pInstance = new CGround(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CGround"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CGround::Clone(void * pArg)
{
	CGround*		pInstance = new CGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CGround"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CGround::Free()
{
	__super::Free();

	SafeRelease(m_pTextureCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
}
