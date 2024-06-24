#include "stdafx.h"

#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "LevelLoading.h"

using namespace Client;

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	SafeAddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	GRAPHICDESC GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWinMode = GRAPHICDESC::MODE_WIN;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->InitializeEngine(LEVEL_END, g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->AddFonts(m_pDevice, m_pContext, TEXT("Font_Nexon"), TEXT("../Bin/Resources/Fonts/129ex.spritefont"))))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	const LEVEL eStartLevel = LEVEL_GAMEPLAY;

	if (FAILED(OpenLevel(eStartLevel)))
		return E_FAIL;
	
	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return ;

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif // _DEBUG

	if (m_pGameInstance->GetDIKDownState(DIK_F1)) {
		g_bDebug = !g_bDebug;
	}

	if (m_pGameInstance->GetDIKDownState(DIK_F3)) {
		m_bShowImGuiDemo = !m_bShowImGuiDemo;
	}

	if (m_pGameInstance->GetDIKDownState(DIK_F4) && m_pGameInstance->GetDIKState(DIK_LSHIFT)) {
		g_bKeyInput = !g_bKeyInput;
	}
	
	m_pGameInstance->TickEngine(fTimeDelta);

}

HRESULT CMainApp::RenderIMGUI()
{
	if(m_bShowImGuiDemo)
		ImGui::ShowDemoWindow(&m_bShowImGuiDemo);

	ImGui::Begin("MainApp");
	ImGui::Text("[KeyInput]"); 

	ImGui::Text("Show Render Target"); ImGui::SameLine(); ImGui::Text("F1");
	ImGui::Text("Change Camera Mode"); ImGui::SameLine(); ImGui::Text("F2");
	ImGui::Text("Show ImGui Demo"); ImGui::SameLine(); ImGui::Text("F3");
	ImGui::Text("Change Input Mode"); ImGui::SameLine(); ImGui::Text("L Shift F4");
	ImGui::Text("Place Player Zero"); ImGui::SameLine(); ImGui::Text("R");

	ImGui::End();
	return S_OK;
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->ClearBackBufferView(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->ClearDepthStencilView();
	
	m_pRenderer->Draw();
	m_pGameInstance->RenderLevel();

	RenderIMGUI();

#ifdef _DEBUG
	++m_iNumDraw;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("에프피에스 : %d"), m_iNumDraw);
		m_iNumDraw = 0;
		m_fTimeAcc = 0.f;
	}


	SetWindowText(g_hWnd, m_szFPS);
#endif // _DEBUG

#ifdef _DEBUG
	if (g_bDebug) {
		m_pRenderer->DebugDraw();
	}

	m_pRenderer->DebugDrawClear();

	m_pGameInstance->RenderTool();
#endif

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::OpenLevel(LEVEL eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevelLoading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->AddPrototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Interval */
	if (FAILED(m_pGameInstance->AddPrototype(LEVEL_STATIC, TEXT("Prototype_Component_Interval"),
		CInterval::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->AddPrototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBufferRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->AddPrototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->AddPrototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->AddPrototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Deferred"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ClientDeferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	SafeAddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	return S_OK;
}

CMainApp* CMainApp::Create()
{	
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CMainApp"));
		SafeRelease(pInstance);
	}
	
	return pInstance;
}

void CMainApp::Free()
{
	SafeRelease(m_pDevice);
	SafeRelease(m_pContext);
	SafeRelease(m_pRenderer);
	SafeRelease(m_pGameInstance);

	CGameInstance::ReleaseEngine();
}
