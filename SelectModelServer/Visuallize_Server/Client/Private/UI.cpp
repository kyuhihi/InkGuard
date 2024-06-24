#include "stdafx.h"
#include "..\Public\UI.h"
#include "GameInstance.h"

CUI::CUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void * pArg)
{
	
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_fPosX = m_fX - g_iWinSizeX * 0.5f;
	m_fPosY = -m_fY + g_iWinSizeY * 0.5f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280.f, 720.f, 0.f, 1.f)));

	return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{
	m_pTransformCom->SetScale(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->SetState(CTransform::STATE_POSITION, XMVectorSet(m_fPosX,m_fPosY, 0.0f, 1.f));
}

void CUI::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI::Render(_uint eRenderGroup)
{
	m_pShaderCom->SetRawValue("g_WorldMatrix", &m_pTransformCom->GetWorldFloat4x4TP(), sizeof(_float4x4));
	m_pShaderCom->SetRawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->SetRawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	return S_OK;
}


void CUI::Free()
{
	__super::Free();

	SafeRelease(m_pTextureCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
}
