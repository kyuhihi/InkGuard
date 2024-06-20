#include "..\Public\GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphicDevice(CGraphicDevice::GetInstance())
	, m_pInputDevice(CInputDevice::GetInstance())
	, m_pLevelManager(CLevelManager::GetInstance())
	, m_pObjectManager(CObjectManager::GetInstance())
	, m_pComponentManager(CComponentManager::GetInstance())
	, m_pTimerManager(CTimerManager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pLightManager(CLightManager::GetInstance())
	, m_pFrustum(CFrustum::GetInstance())
	, m_pFontManager(CFontManager::GetInstance())
	, m_pTargetManager(CTargetManager::GetInstance())
	, m_pSoundManager(CSoundManager::GetInstance())
	, m_pPicking(CPicking::GetInstance())

#ifdef _DEBUG
	, m_pImGuiManager(CImGuiManager::GetInstance())
#endif // _DEBUG
{
	SafeAddRef(m_pTargetManager);
	SafeAddRef(m_pFontManager);
	SafeAddRef(m_pFrustum);
	SafeAddRef(m_pLightManager);
	SafeAddRef(m_pPipeLine);
	SafeAddRef(m_pTimerManager);
	SafeAddRef(m_pComponentManager);
	SafeAddRef(m_pObjectManager);
	SafeAddRef(m_pLevelManager);
	SafeAddRef(m_pInputDevice);
	SafeAddRef(m_pSoundManager);
	SafeAddRef(m_pGraphicDevice);
	SafeAddRef(m_pPicking);

#ifdef _DEBUG
	SafeAddRef(m_pImGuiManager);
#endif // _DEBUG

}


HRESULT CGameInstance::InitializeEngine(_uint iNumLevels, HINSTANCE hInst, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	/* 엔진을 구동하기위한 초기화작업을 거치낟. */
	if (nullptr == m_pGraphicDevice)
		return E_FAIL;

	/* 그래픽디바이스 초기화. */
	if (FAILED(m_pGraphicDevice->ReadyGraphicDevice(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDevice, ppContext)))
		return E_FAIL;

	/* 입력 초기화. */
	if (FAILED(m_pInputDevice->Initialize(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	/* 피킹 초기화. */
	if (FAILED(m_pPicking->Initialize(GraphicDesc.hWnd, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY)))
		return E_FAIL;
		
	if (FAILED(m_pObjectManager->ReserveContainer(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponentManager->ReserveContainer(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

#ifdef _DEBUG
	if (::AllocConsole() == TRUE) {
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		std::ios::sync_with_stdio();
	}

	if (FAILED(m_pImGuiManager->Initialize(GraphicDesc.hWnd, *ppDevice, *ppContext)))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

void CGameInstance::TickEngine(_float fTimeDelta)
{
	if (nullptr == m_pLevelManager || 
		nullptr == m_pObjectManager || 
		nullptr == m_pInputDevice)
		return;

	m_pInputDevice->Update();

#ifdef _DEBUG
	m_pImGuiManager->FrameBegin();
#endif // _DEBUG

	m_pLevelManager->LevelOpenReady(false);

	m_pLevelManager->Tick(fTimeDelta);

	m_pObjectManager->Tick(fTimeDelta);

	m_pPipeLine->Update();

	m_pFrustum->Tick();

	m_pPicking->Tick();

	m_pLevelManager->LevelOpenReady(true);

	m_pObjectManager->LateTick(fTimeDelta);

	m_pLevelManager->LateTick(fTimeDelta);

}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObjectManager)
		return;

	m_pComponentManager->Clear(iLevelIndex);

	m_pObjectManager->Clear(iLevelIndex);
}

HRESULT CGameInstance::ClearBackBufferView(_float4 vClearColor)
{
	if (nullptr == m_pGraphicDevice)
		return E_FAIL;

	return m_pGraphicDevice->ClearBackBufferView(vClearColor);	
}

HRESULT CGameInstance::ClearDepthStencilView()
{
	if (nullptr == m_pGraphicDevice)
		return E_FAIL;

	return m_pGraphicDevice->ClearDepthStencilView();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphicDevice)
		return E_FAIL;

	return m_pGraphicDevice->Present();
}

HRESULT CGameInstance::OpenLevel(_uint iLevelIndex, CLevel* pNewLevel)
{
	if (nullptr == m_pLevelManager)
		return E_FAIL;

	return m_pLevelManager->OpenLevel(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::RenderLevel()
{
	if (nullptr == m_pLevelManager)
		return E_FAIL;

	return m_pLevelManager->Render();	
}

CComponent * CGameInstance::GetComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iLayerIndex)
{
	if (nullptr == m_pObjectManager)
		return nullptr;

	return m_pObjectManager->GetComponentPtr(iLevelIndex, pLayerTag, pComponentTag, iLayerIndex);	
}

CGameObject* CGameInstance::CloneGameObject(const _tchar* pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObjectManager)
		return nullptr;

	return m_pObjectManager->CloneGameObject(pPrototypeTag, pArg);	
}

HRESULT CGameInstance::AddPrototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObjectManager)
		return E_FAIL;

	return m_pObjectManager->AddPrototype(pPrototypeTag, pPrototype);	
}

CGameObject* CGameInstance::AddGameObjectToLayer(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObjectManager)
		return nullptr;

	return m_pObjectManager->AddGameObjectToLayer(pPrototypeTag, iLevelIndex, pLayerTag, pArg);
}

HRESULT CGameInstance::AddPrototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponentManager)
		return E_FAIL;

	return m_pComponentManager->AddPrototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::CloneComponent(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponentManager)
		return nullptr;

	return m_pComponentManager->CloneComponent(iLevelIndex, pPrototypeTag, pArg);	
}

_float CGameInstance::GetTimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimerManager)
		return 0.0f;
	return m_pTimerManager->GetTimeDelta(pTimerTag);
}

HRESULT CGameInstance::AddTimer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimerManager)
		return E_FAIL;
	return m_pTimerManager->AddTimer(pTimerTag);
}

HRESULT CGameInstance::UpdateTimer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimerManager)
		return E_FAIL;

	return m_pTimerManager->UpdateTimer(pTimerTag);
}

_char CGameInstance::GetDIKState(_uchar eKeyID)
{
	if (nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->GetDIKState(eKeyID);	
}

_bool CGameInstance::GetDIKDownState(_uchar eKeyID)
{
	if (nullptr == m_pInputDevice)
		return false;

	return m_pInputDevice->GetDIKDownState(eKeyID);
}

_bool CGameInstance::GetDIKUPState(_uchar eKeyID) {
	if (nullptr == m_pInputDevice)
		return false;

	return m_pInputDevice->GetDIKUPState(eKeyID);
}

_char CGameInstance::GetDIMKeyState(DIMK eMouseKeyID)
{
	if (nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->GetDIMKeyState(eMouseKeyID);
}

_long CGameInstance::GetDIMMoveState(DIMM eMouseMoveID)
{
	if (nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->GetDIMMoveState(eMouseMoveID);
}

_bool CGameInstance::GetDIMKeyDownState(DIMK eMouseKeyID)
{
	if (nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->GetDIMKeyDownState(eMouseKeyID);
}

void CGameInstance::SetTransform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->SetTransform(eTransformState, TransformMatrix);
}

_matrix CGameInstance::GetTransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->GetTransformMatrix(eTransformState);	
}

_float4x4 CGameInstance::GetTransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->GetTransformFloat4x4(eTransformState);
	
}

_float4x4 CGameInstance::GetTransformFloat4x4TP(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->GetTransformFloat4x4TP(eTransformState);
}

const _float CGameInstance::GetProjectionFar() const
{
	if (nullptr == m_pPipeLine)
		return _float();

	return m_pPipeLine->GetProjectionFar();
}

const _float CGameInstance::GetProjectionNear() const
{
	if (nullptr == m_pPipeLine)
		return _float();

	return m_pPipeLine->GetProjectionNear();
}

_float4 CGameInstance::GetCamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->GetCamPosition();	
}


const LIGHTDESC * CGameInstance::GetLightDesc(_uint iIndex)
{
	if (nullptr == m_pLightManager)
		return nullptr;

	return m_pLightManager->GetLightDesc(iIndex);	
}

CLight* CGameInstance::AddLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLightManager)
		return nullptr;

	return m_pLightManager->AddLight(pDevice, pContext, LightDesc);	
}

_bool CGameInstance::isInFrustumWorldSpace(_fvector vWorldPos, float fRadius)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isInWorldSpace(vWorldPos, fRadius);
}

HRESULT CGameInstance::AddFonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr == m_pFontManager)
		return E_FAIL;

	return m_pFontManager->AddFonts(pDevice, pContext, pFontTag, pFontFilePath);	
}

HRESULT CGameInstance::RenderFonts(const _tchar * pFontTag, const _tchar * pTextm, _float2 vPosition, _fvector vColor, _float fAngle, _float2 vOrigin, _float2 vScale)
{
	if (nullptr == m_pFontManager)
		return E_FAIL;

	return m_pFontManager->RenderFonts(pFontTag, pTextm, vPosition, vColor, fAngle, vOrigin, vScale);	
}

HRESULT CGameInstance::AddRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	return m_pTargetManager->AddRenderTarget(pDevice, pContext, pTargetTag, iSizeX, iSizeY, eFormat, pClearColor);
}

HRESULT CGameInstance::AddMRT(const _tchar* pMRTTag, const _tchar* pTargetTag)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	return m_pTargetManager->AddMRT(pMRTTag, pTargetTag);
}

HRESULT CGameInstance::BindRenderTargetSRV(const _tchar * pTargetTag, CShader * pShader, const char * pConstantName)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	return m_pTargetManager->BindSRV(pTargetTag, pShader, pConstantName);	
}

HRESULT CGameInstance::ClearRenderTargetSRV(ID3D11DeviceContext* pContext)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	return m_pTargetManager->ClearSRV(pContext);
}

HRESULT CGameInstance::BeginMRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDepthStencil)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	return m_pTargetManager->BeginMRT(pContext, pMRTTag, pDepthStencil);
}

HRESULT CGameInstance::BeginOverwriteMRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDepthStencil)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	return m_pTargetManager->BeginOverwriteMRT(pContext, pMRTTag, pDepthStencil);

}


HRESULT CGameInstance::EndMRT(ID3D11DeviceContext* pContext)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	return m_pTargetManager->EndMRT(pContext);
}

HRESULT CGameInstance::AddDebugTarget(const _tchar* pTargetTag, _uint iCol, _uint iRow)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	return m_pTargetManager->AddDebugTarget(pTargetTag, iCol, iRow);
}

HRESULT CGameInstance::AddDebugMRT(const _tchar* pMRTTag)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	return m_pTargetManager->AddDebugMRT(pMRTTag);
}

HRESULT CGameInstance::PlaySoundChannel(_tchar* pFileName, SOUND_CHANNEL eID, float volume) 
{
	if (nullptr == m_pSoundManager)
		return E_FAIL;

	m_pSoundManager->PlaySoundChannel(pFileName, eID, volume);

	return S_OK;
}

HRESULT CGameInstance::StopSound(SOUND_CHANNEL eID) 
{
	if (nullptr == m_pSoundManager)
		return E_FAIL;

	m_pSoundManager->StopSound(eID);

	return S_OK;
}

HRESULT CGameInstance::PlayBGM(_tchar* pFileName, float volume) 
{
	if (nullptr == m_pSoundManager)
		return E_FAIL;

	m_pSoundManager->PlayBGM(pFileName, volume);

	return S_OK;
}

HRESULT CGameInstance::StopBGM() {
	if (nullptr == m_pSoundManager)
		return E_FAIL;

	m_pSoundManager->StopBGM();

	return S_OK;
}

HRESULT CGameInstance::PlayOneShotEffect(_tchar * pFileName, float volume) 
{
	if (nullptr == m_pSoundManager)
		return E_FAIL;

	m_pSoundManager->PlayOneShotEffect(pFileName, volume);

	return S_OK;
}

HRESULT CGameInstance::PlayEffect(_tchar* pFileName, float volume) 
{
	if (nullptr == m_pSoundManager)
		return E_FAIL;

	m_pSoundManager->PlayEffect(pFileName, volume);

	return S_OK;
}

HRESULT CGameInstance::StopEffect() {
	if (nullptr == m_pSoundManager)
		return E_FAIL;

	m_pSoundManager->StopEffect();

	return S_OK;
}

#ifdef _DEBUG
HRESULT CGameInstance::RenderTool()
{
	if (nullptr == m_pSoundManager)
		return E_FAIL;

	m_pImGuiManager->Render();

	return S_OK;
}
#endif // _DEBUG

void CGameInstance::ReleaseEngine()
{
	CGameInstance::GetInstance()->DestroyInstance();

	CComponentManager::GetInstance()->DestroyInstance();

	CObjectManager::GetInstance()->DestroyInstance();

	CLevelManager::GetInstance()->DestroyInstance();

	CTimerManager::GetInstance()->DestroyInstance();

	CPicking::GetInstance()->DestroyInstance();

	CLightManager::GetInstance()->DestroyInstance();

	CFrustum::GetInstance()->DestroyInstance();

	CInputDevice::GetInstance()->DestroyInstance();	

	CFontManager::GetInstance()->DestroyInstance();

	CTargetManager::GetInstance()->DestroyInstance();

	CPipeLine::GetInstance()->DestroyInstance();

	CSoundManager::GetInstance()->DestroyInstance();

#ifdef _DEBUG
	CImGuiManager::GetInstance()->DestroyInstance();
#endif // _DEBUG
	
	CGraphicDevice::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	SafeRelease(m_pTargetManager);
	SafeRelease(m_pFontManager);
	SafeRelease(m_pFrustum);
	SafeRelease(m_pPicking);
	SafeRelease(m_pLightManager);
	//SafeRelease(m_pPipeLine);
	SafeRelease(m_pTimerManager);
	SafeRelease(m_pComponentManager);
	SafeRelease(m_pObjectManager);
	SafeRelease(m_pLevelManager);
	SafeRelease(m_pInputDevice);
	SafeRelease(m_pSoundManager);

#ifdef _DEBUG
	SafeRelease(m_pImGuiManager);
#endif // _DEBUG


	SafeRelease(m_pGraphicDevice); 
	//CPipeLine::DestroyInstance();
	//m_pPipeLine = nullptr;
	SafeRelease(m_pPipeLine);

}
