#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "TargetManager.h"
#include "Shader.h"
#include "VIBufferRect.h"
#include "LightManager.h"
#include "PipeLine.h"
#include "Texture.h"
#include "LightManager.h"
#include "Light.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pTargetManager(CTargetManager::GetInstance())
	, m_pLightManager(CLightManager::GetInstance())
{
	SafeAddRef(m_pLightManager);
	SafeAddRef(m_pTargetManager);
}

HRESULT CRenderer::InitializePrototype()
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	_uint iNumViewport = 1;
	D3D11_VIEWPORT viewportDesc;
	m_pContext->RSGetViewports(&iNumViewport, &viewportDesc);

	_uint iWidth = static_cast<_uint>(viewportDesc.Width);
	_uint iHeight = static_cast<_uint>(viewportDesc.Height);

	_uint iShadowWidth = 4000; // 8000
	_uint iShadowHeight = 2000; // 8000

	m_pShadowDepthStencil = m_pTargetManager->CreateDepthStencil(m_pDevice, iShadowWidth, iShadowHeight);
	m_pShadowDepthViewport = m_pTargetManager->CreateViewPort(iShadowWidth, iShadowHeight);

	/* For.Target_Diffuse */
	if (FAILED(m_pTargetManager->AddRenderTarget(
		m_pDevice, m_pContext, TEXT("Target_Diffuse"),
		iWidth, iHeight, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTargetManager->AddRenderTarget(
		m_pDevice, m_pContext, TEXT("Target_Normal"),
		iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pTargetManager->AddRenderTarget(
		m_pDevice, m_pContext, TEXT("Target_Depth"),
		iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 1.0f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shader */
	if (FAILED(m_pTargetManager->AddRenderTarget(
		m_pDevice, m_pContext, TEXT("Target_Shade"),
		iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTargetManager->AddRenderTarget(
		m_pDevice, m_pContext, TEXT("Target_Specular"),
		iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->AddRenderTarget(
		m_pDevice, m_pContext, TEXT("Target_Shadow_Depth"),
		iShadowWidth, iShadowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(1.f, 1.f, 1.f, 1.f)))) {
		return E_FAIL;
	}

	if (FAILED(m_pTargetManager->AddRenderTarget(
		m_pDevice, m_pContext, TEXT("Target_Shadow"),
		iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.f, 0.f, 0.f, 0.f)))) {
		return E_FAIL;
	}

	if (FAILED(m_pTargetManager->AddRenderTarget(
		m_pDevice, m_pContext, TEXT("Target_Blend"),
		iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.f, 0.f, 0.f, 1.f)))) {
		return E_FAIL;
	}


	/* For.MRT_Deferred */
	if (FAILED(m_pTargetManager->AddMRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;

	/* For.MRT_LightAcc */
	if (FAILED(m_pTargetManager->AddMRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTargetManager->AddMRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	/* For.MRT_Shadow */
	if (FAILED(m_pTargetManager->AddMRT(TEXT("MRT_Shadow_Depth"), TEXT("Target_Shadow_Depth")))) {
		return E_FAIL;
	}

	if (FAILED(m_pTargetManager->AddMRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow")))) {
		return E_FAIL;
	}

	/* For.Blend */
	if (FAILED(m_pTargetManager->AddMRT(TEXT("MRT_Blend"), TEXT("Target_Blend")))) {
		return E_FAIL;
	}

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

#ifdef _DEBUG
	m_pTargetManager->AddDebugTarget(TEXT("Target_Diffuse"), 0, 0);
	m_pTargetManager->AddDebugTarget(TEXT("Target_Normal"), 1, 0);
	m_pTargetManager->AddDebugTarget(TEXT("Target_Depth"), 2, 0);

	m_pTargetManager->AddDebugTarget(TEXT("Target_Shade"), 0, 1);
	m_pTargetManager->AddDebugTarget(TEXT("Target_Specular"), 1, 1);
	m_pTargetManager->AddDebugTarget(TEXT("Target_Shadow_Depth"), 2, 1);
	m_pTargetManager->AddDebugTarget(TEXT("Target_Shadow"), 3, 1);

	m_pTargetManager->AddDebugTarget(TEXT("Target_Blend"), 0, 2);

	m_pTargetManager->AddDebugMRT(TEXT("MRT_Deferred"));
	m_pTargetManager->AddDebugMRT(TEXT("MRT_LightAcc"));
	m_pTargetManager->AddDebugMRT(TEXT("MRT_Shadow_Depth"));
	m_pTargetManager->AddDebugMRT(TEXT("MRT_Shadow"));
	m_pTargetManager->AddDebugMRT(TEXT("MRT_Blend"));

	m_pVIBuffer = CVIBufferRect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif // _DEBUG

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(viewportDesc.Width, viewportDesc.Height, 0.f, 1.f)));


	return S_OK;
}

HRESULT CRenderer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CRenderer::AddRenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);


	SafeAddRef(pGameObject);

	return S_OK;
}

void CRenderer::MinusSSDTimes()
{
	if (m_SSDs.empty())
		return;

	_uint iPopCount{ 0 };
	for (auto& iter : m_SSDs)
	{
		iter.fLiveTime -= m_fTimeDelta;
		if (iter.fLiveTime <= 0.f)
			++iPopCount;
	}

	if (iPopCount == 0)
		return;

	for (_uint i = 0; i < iPopCount; ++i)
		m_SSDs.pop_front();

}

HRESULT CRenderer::Draw()
{
	MinusSSDTimes();

	if (FAILED(RenderPriority()))
		return E_FAIL;

	if (FAILED(RenderNonAlphaBlend()))
		return E_FAIL;

	if (FAILED(RenderLights()))
		return E_FAIL;

	if (FAILED(RenderBlend()))
		return E_FAIL;

	RenderShadow();

	if (FAILED(RenderNonLight()))
		return E_FAIL;

	if (FAILED(RenderAlphaBlend()))
		return E_FAIL;

	if (FAILED(RenderBackBuffer()))
		return E_FAIL;

	if (FAILED(RenderUI()))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::AddDebugGroup(CComponent* pDebugCom)
{
	m_DebugObject.push_back(pDebugCom);

	SafeAddRef(pDebugCom);

	return S_OK;
}

HRESULT CRenderer::DebugDraw()
{
	if (FAILED(RenderDebug()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::DebugDrawClear()
{
	for (auto& pDebugCom : m_DebugObject)
	{
		pDebugCom->Render();
		SafeRelease(pDebugCom);
	}

	m_DebugObject.clear();

	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderer::RenderPriority()
{
	if (FAILED(m_pTargetManager->BeginMRT(m_pContext, TEXT("MRT_Blend"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pRenderObject && pRenderObject->GetVisible())
			pRenderObject->Render(RENDER_PRIORITY);

		SafeRelease(pRenderObject);
	}
	m_RenderObjects[RENDER_PRIORITY].clear();

	if (FAILED(m_pTargetManager->EndMRT(m_pContext)))
		return E_FAIL;

	m_pTargetManager->ClearSRV(m_pContext);

	return S_OK;
}

HRESULT CRenderer::RenderNonAlphaBlend()
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	if (FAILED(m_pTargetManager->BeginMRT(m_pContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONALPHABLEND])
	{
		if (nullptr != pRenderObject && pRenderObject->GetVisible())
			pRenderObject->Render(RENDER_NONALPHABLEND);

		SafeRelease(pRenderObject);
	}
	m_RenderObjects[RENDER_NONALPHABLEND].clear();

	if (FAILED(m_pTargetManager->EndMRT(m_pContext)))
		return E_FAIL;

	m_pTargetManager->ClearSRV(m_pContext);

	return S_OK;
}

HRESULT CRenderer::RenderLights()
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	/* Target_Shader타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTargetManager->BeginMRT(m_pContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	_float4x4 WorldMatrix;

	_uint iNumViewport = 1;
	D3D11_VIEWPORT ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->SetRawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_ViewMatrix", &m_viewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_ProjMatrix", &m_projMatrix, sizeof(_float4x4))))
		return E_FAIL;

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4 ViewMatrixInv;
	_float4x4 ProjMatrixInv;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(pPipeLine->GetTransformMatrixInverse(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(pPipeLine->GetTransformMatrixInverse(CPipeLine::D3DTS_PROJ)));

	if (FAILED(m_pShader->SetRawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_vCamPosition", &pPipeLine->GetCamPosition(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	m_pLightManager->Render(m_pShader, m_pVIBuffer);

	if (FAILED(m_pTargetManager->EndMRT(m_pContext)))
		return E_FAIL;

	m_pTargetManager->ClearSRV(m_pContext);

	return S_OK;
}

HRESULT CRenderer::RenderBlend()
{

	for (auto& pRenderObject : m_RenderObjects[RENDER_LIGHTBLEND])
	{
		if (nullptr != pRenderObject && pRenderObject->GetVisible())
			pRenderObject->Render(RENDER_LIGHTBLEND);

		SafeRelease(pRenderObject);
	}
	m_RenderObjects[RENDER_LIGHTBLEND].clear();

	//_float4x4			WorldMatrix;

	//_uint				iNumViewport = 1;
	//D3D11_VIEWPORT		ViewportDesc;

	//m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	//XMStoreFloat4x4(&WorldMatrix,
	//	XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	//if (FAILED(m_pTargetManager->BeginOverwriteMRT(m_pContext, TEXT("MRT_Blend"))))
	//	return E_FAIL;

	//if (FAILED(m_pShader->SetRawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	//	return E_FAIL;
	//if (FAILED(m_pShader->SetRawValue("g_ViewMatrix", &m_viewMatrix, sizeof(_float4x4))))
	//	return E_FAIL;
	//if (FAILED(m_pShader->SetRawValue("g_ProjMatrix", &m_projMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	//if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
	//	return E_FAIL;

	//if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
	//	return E_FAIL;

	//if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
	//	return E_FAIL;

	//m_pShader->Begin(TECHNIQUE_DEFAULT, DEFAULT_BLEND);

	//m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->BeginOverwriteMRT(m_pContext, TEXT("MRT_Blend"))))
		return E_FAIL;

	if (!m_SSDs.empty())
	{
		for (auto& iter : m_SSDs)
		{
			_float4x4 vSSDWorldInv;
			XMStoreFloat4x4(&vSSDWorldInv, XMMatrixTranspose(XMMatrixInverse(nullptr, iter.vObjectMatrix)));
			if (FAILED(iter.pTexture->SetSRV(m_pShader, "g_SSDTexture", iter.iTextureIndex)))
				return E_FAIL;

			if (FAILED(m_pShader->SetRawValue("g_WorldMatrixInv", &vSSDWorldInv, sizeof(_float4x4))))
				return E_FAIL;

			if (FAILED(m_pShader->SetRawValue("g_fSSDAlpha", &iter.fLiveTime, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShader->SetRawValue("g_vSSDSize", &iter.vSize, sizeof(_float2))))
				return E_FAIL;

			m_pShader->Begin(1, 0);//SSD default
			m_pVIBuffer->Render();
		}
	}

	if (FAILED(m_pTargetManager->EndMRT(m_pContext)))
		return E_FAIL;

	m_pTargetManager->ClearSRV(m_pContext);

	return S_OK;
}

HRESULT CRenderer::RenderNonLight()
{
	if (FAILED(m_pTargetManager->BeginOverwriteMRT(m_pContext, TEXT("MRT_Blend"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pRenderObject && pRenderObject->GetVisible())
			pRenderObject->Render(RENDER_NONLIGHT);

		SafeRelease(pRenderObject);
	}
	m_RenderObjects[RENDER_NONLIGHT].clear();

	if (FAILED(m_pTargetManager->EndMRT(m_pContext)))
		return E_FAIL;

	m_pTargetManager->ClearSRV(m_pContext);

	return S_OK;
}

HRESULT CRenderer::RenderAlphaBlend()
{
	if (FAILED(m_pTargetManager->BeginOverwriteMRT(m_pContext, TEXT("MRT_Blend"))))
		return E_FAIL;

	m_RenderObjects[RENDER_ALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)
		{
			return pSour->GetCamDistance() > pDest->GetCamDistance();
		});

	for (auto& pRenderObject : m_RenderObjects[RENDER_ALPHABLEND])
	{
		if (nullptr != pRenderObject && pRenderObject->GetVisible())
			pRenderObject->Render(RENDER_ALPHABLEND);

		SafeRelease(pRenderObject);
	}
	m_RenderObjects[RENDER_ALPHABLEND].clear();

	if (FAILED(m_pTargetManager->EndMRT(m_pContext)))
		return E_FAIL;

	m_pTargetManager->ClearSRV(m_pContext);


	return S_OK;
}

HRESULT CRenderer::RenderUI()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject && pRenderObject->GetVisible())
			pRenderObject->Render(RENDER_UI);

		SafeRelease(pRenderObject);
	}
	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::RenderShadow()
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	_uint iNumViewport = 1;
	D3D11_VIEWPORT viewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &viewportDesc);

	_matrix worldMatrix = XMMatrixTranspose(XMMatrixScaling(viewportDesc.Width, viewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f));

	_uint iNumLights = m_pLightManager->GetNumLights();

	for (_uint i = 0; i < iNumLights; ++i) {
		CLight* pLight = m_pLightManager->GetLight(i);

		if (FAILED(RenderShadowDepth(pLight)))
			return E_FAIL;

		if (FAILED(RenderShadowColor(worldMatrix, pLight)))
			return E_FAIL;

		if (FAILED(RenderShadowBlend(worldMatrix, pLight)))
			return E_FAIL;
	}

	for (auto& pRenderObject : m_RenderObjects[RENDER_SHADOW_DEPTH]) {
		SafeRelease(pRenderObject);
	}

	m_RenderObjects[RENDER_SHADOW_DEPTH].clear();

	m_pContext->RSSetViewports(1, &viewportDesc);

	m_pTargetManager->ClearSRV(m_pContext);

	return S_OK;
}

HRESULT CRenderer::RenderBackBuffer()
{
	_float4x4 worldMatrix;

	_uint iNumViewport = 1;
	D3D11_VIEWPORT viewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &viewportDesc);

	XMStoreFloat4x4(&worldMatrix,
		XMMatrixTranspose(XMMatrixScaling(viewportDesc.Width, viewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader->SetRawValue("g_WorldMatrix", &worldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_ViewMatrix", &m_viewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_ProjMatrix", &m_projMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Blend"), m_pShader, "g_BlendTexture")))
		return E_FAIL;

	m_pShader->Begin(TECHNIQUE_DEFAULT, DEFAULT_BACKBUFFER);

	m_pVIBuffer->Render();

	for (auto& pRenderObject : m_RenderObjects[RENDER_BACKBUFFER]) {
		if (nullptr != pRenderObject && pRenderObject->GetVisible())
			pRenderObject->Render(RENDER_BACKBUFFER);

		SafeRelease(pRenderObject);
	}

	m_RenderObjects[RENDER_BACKBUFFER].clear();

	m_pTargetManager->ClearSRV(m_pContext);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::RenderDebug()
{
	m_pShader->SetRawValue("g_ViewMatrix", &m_viewMatrix, sizeof(_float4x4));
	m_pShader->SetRawValue("g_ProjMatrix", &m_projMatrix, sizeof(_float4x4));

	m_pTargetManager->RenderDebug(m_pVIBuffer, m_pShader);

	for (auto& pDebugCom : m_DebugObject)
	{
		pDebugCom->Render();
	}

	m_pTargetManager->ClearSRV(m_pContext);

	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderer::RenderShadowDepth(CLight* pLight)
{
	if (!pLight || m_RenderObjects[RENDER_SHADOW_DEPTH].size() <= 0)
		return E_FAIL;

	_uint iNumViewport = 1;
	D3D11_VIEWPORT viewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &viewportDesc);
	m_pContext->RSSetViewports(1, &m_pShadowDepthViewport);
	if (FAILED(m_pTargetManager->BeginMRT(m_pContext, TEXT("MRT_Shadow_Depth"), m_pShadowDepthStencil)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_SHADOW_DEPTH]) {
		if (nullptr != pRenderObject && pRenderObject->GetVisible()) {
			if (FAILED(pRenderObject->RenderLightDepth(pLight))) {
				continue;
			}
		}
	}

	if (FAILED(m_pTargetManager->EndMRT(m_pContext)))
		return E_FAIL;

	m_pContext->RSSetViewports(1, &viewportDesc);

	return S_OK;
}

HRESULT CRenderer::RenderShadowColor(_matrix worldMatrix, CLight* pLight)
{
	if (FAILED(m_pTargetManager->BeginMRT(m_pContext, TEXT("MRT_Shadow"))))
		return E_FAIL;

	_float4x4 passWorldMatrix;
	XMStoreFloat4x4(&passWorldMatrix, XMMatrixTranspose(worldMatrix));

	if (FAILED(m_pShader->SetRawValue("g_WorldMatrix", &passWorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_ViewMatrix", &m_viewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_ProjMatrix", &m_projMatrix, sizeof(_float4x4))))
		return E_FAIL;

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	_float4x4 viewMatrixInv;
	_float4x4 projMatrixInv;

	XMStoreFloat4x4(&viewMatrixInv, XMMatrixTranspose(pPipeLine->GetTransformMatrixInverse(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&projMatrixInv, XMMatrixTranspose(pPipeLine->GetTransformMatrixInverse(CPipeLine::D3DTS_PROJ)));

	_float fProjFar = pPipeLine->GetProjectionFar();

	if (FAILED(m_pShader->SetRawValue("g_ViewMatrixInv", &viewMatrixInv, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_ProjMatrixInv", &projMatrixInv, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fProjFar", &fProjFar, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Shadow_Depth"), m_pShader, "g_ShadowDepthTexture")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_LightViewMatrix", &pLight->GetViewMatrixTP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_LightProjMatrix", &pLight->GetProjectionMatrixTP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_vLightPos", &pLight->GetLightDesc()->vPosition, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fBias", &pLight->GetLightDesc()->fBias, sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(TECHNIQUE_SHADOW, SHADOW_SHADOW);

	m_pVIBuffer->Render();

	if (FAILED(m_pTargetManager->EndMRT(m_pContext)))
		return E_FAIL;

	m_pTargetManager->ClearSRV(m_pContext);

	return S_OK;
}

HRESULT CRenderer::RenderShadowBlend(_matrix worldMatrix, CLight* pLight)
{
	if (FAILED(m_pTargetManager->BeginOverwriteMRT(m_pContext, TEXT("MRT_Blend"))))
		return E_FAIL;

	_float4x4 passWorldMatrix;
	XMStoreFloat4x4(&passWorldMatrix, XMMatrixTranspose(worldMatrix));

	if (FAILED(m_pShader->SetRawValue("g_WorldMatrix", &passWorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_ViewMatrix", &m_viewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_ProjMatrix", &m_projMatrix, sizeof(_float4x4))))
		return E_FAIL;

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	_float4x4 viewMatrixInv;
	_float4x4 projMatrixInv;

	XMStoreFloat4x4(&viewMatrixInv, XMMatrixTranspose(pPipeLine->GetTransformMatrixInverse(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&projMatrixInv, XMMatrixTranspose(pPipeLine->GetTransformMatrixInverse(CPipeLine::D3DTS_PROJ)));

	_float fProjFar = pPipeLine->GetProjectionFar();

	if (FAILED(m_pShader->SetRawValue("g_ViewMatrixInv", &viewMatrixInv, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_ProjMatrixInv", &projMatrixInv, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fProjFar", &fProjFar, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pTargetManager->BindSRV(TEXT("Target_Shadow"), m_pShader, "g_ShadowTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_LightViewMatrix", &pLight->GetViewMatrixTP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_LightProjMatrix", &pLight->GetProjectionMatrixTP(), sizeof(_float4x4))))
		return E_FAIL;

	const LIGHTDESC* lightDesc = pLight->GetLightDesc();

	if (FAILED(m_pShader->SetRawValue("g_vLightPos", &lightDesc->vPosition, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_vLightAmbient", &lightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_vLightDiffuse", &lightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(TECHNIQUE_SHADOW, SHADOW_BLEND)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pTargetManager->EndMRT(m_pContext)))
		return E_FAIL;

	m_pTargetManager->ClearSRV(m_pContext);

	return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRenderer"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	for (auto& List : m_RenderObjects)
	{
		for (auto& pGameObject : List)
		{
			SafeRelease(pGameObject);
		}
		List.clear();
	}

#ifdef _DEBUG
	SafeRelease(m_pShader);
	SafeRelease(m_pVIBuffer);
#endif // _DEBUG

	SafeRelease(m_pLightManager);
	SafeRelease(m_pTargetManager);
	SafeRelease(m_pShadowDepthStencil);
}
