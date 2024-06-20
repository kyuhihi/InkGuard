#include "stdafx.h"
#include "..\Public\Deferred.h"
#include "GameInstance.h"

CDeferred::CDeferred(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_pShaderComponent(nullptr)
	, m_pRendererComponent(nullptr)
	, m_pVIBufferComponent(nullptr)
{
}

CDeferred::CDeferred(const CDeferred& rhs)
	: CGameObject(rhs)
	, m_pShaderComponent(nullptr)
	, m_pRendererComponent(nullptr)
	, m_pVIBufferComponent(nullptr)
{
}

HRESULT CDeferred::InitializePrototype()
{
	return S_OK;
}

HRESULT CDeferred::Initialize(void * pArg)
{
	if (FAILED(ReadyComponents()))
		return E_FAIL;

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(static_cast<_float>(g_iWinSizeX), static_cast<_float>(g_iWinSizeY), 0.f, 1.f)));

	return S_OK;
}

void CDeferred::Tick(_float fTimeDelta)
{

}

void CDeferred::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererComponent)
		return;

	m_pRendererComponent->AddRenderGroup(CRenderer::RENDER_LIGHTBLEND, this);
}

HRESULT CDeferred::Render(_uint eRenderGroup)
{
	if (nullptr == m_pVIBufferComponent ||
		nullptr == m_pShaderComponent)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float4x4 WorldMatrix;

	_uint iNumViewport = 1;
	D3D11_VIEWPORT ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(pGameInstance->BeginOverwriteMRT(m_pContext, TEXT("MRT_Blend"))))
		return E_FAIL;

	if (FAILED(m_pShaderComponent->SetRawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderComponent->SetRawValue("g_ViewMatrix", &m_viewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderComponent->SetRawValue("g_ProjMatrix", &m_projMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(pGameInstance->BindRenderTargetSRV(TEXT("Target_Diffuse"), m_pShaderComponent, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(pGameInstance->BindRenderTargetSRV(TEXT("Target_Shade"), m_pShaderComponent, "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(pGameInstance->BindRenderTargetSRV(TEXT("Target_Specular"), m_pShaderComponent, "g_SpecularTexture")))
		return E_FAIL;

	m_pShaderComponent->Begin(TECHNIQUE_DEFAULT, DEFAULT_BLEND);

	m_pVIBufferComponent->Render();

	pGameInstance->EndMRT(m_pContext);
	pGameInstance->ClearRenderTargetSRV(m_pContext);

	return S_OK;
}


HRESULT CDeferred::ReadyComponents()
{
	/* For.Com_Renderer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererComponent)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Deferred"), TEXT("Com_Shader"), (CComponent**)&m_pShaderComponent)))
		return E_FAIL;

	/* For.Com_Rect */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferComponent)))
		return E_FAIL;

	return S_OK;
}

CDeferred* CDeferred::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDeferred* pInstance = new CDeferred(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CForkLift"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CDeferred::Clone(void * pArg)
{
	CDeferred* pInstance = new CDeferred(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDeferred"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CDeferred::Free()
{
	__super::Free();

	SafeRelease(m_pVIBufferComponent);
	SafeRelease(m_pShaderComponent);
	SafeRelease(m_pRendererComponent);
}
