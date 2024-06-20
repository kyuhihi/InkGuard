#pragma once

#include "TimerManager.h"
#include "GraphicDevice.h"
#include "InputDevice.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "ComponentManager.h"
#include "LightManager.h"
#include "PipeLine.h"
#include "Frustum.h"
#include "FontManager.h"
#include "TargetManager.h"
#include "SoundManager.h"
#include "Picking.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif // _DEBUG

BEGIN(Engine)
class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	HRESULT InitializeEngine(_uint iNumLevel, HINSTANCE hInst, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void TickEngine(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
	

public: /* For.Graphic_Device */
	HRESULT ClearBackBufferView(_float4 vClearColor);
	HRESULT ClearDepthStencilView();
	HRESULT Present();


public: /* For.Level_Manager */				
	HRESULT OpenLevel(_uint iLevelIndex, class CLevel* pNewLevel);
	HRESULT RenderLevel();

public: /* For.Object_Manager */
	class CComponent* GetComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iLayerIndex);
	class CGameObject* CloneGameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT AddPrototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	CGameObject* AddGameObjectToLayer(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr);

public: /*For.Component_Manager*/
	HRESULT AddPrototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* CloneComponent(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

public: /* for.Timer_Manager */
	_float GetTimeDelta(const _tchar* pTimerTag);
	HRESULT AddTimer(const _tchar* pTimerTag);
	HRESULT UpdateTimer(const _tchar* pTimerTag);	

public: /* For.Input_Device */
	_char GetDIKState(_uchar eKeyID);
	_bool GetDIKDownState(_uchar eKeyID);
	_bool GetDIKUPState(_uchar eKeyID);

	_char GetDIMKeyState(DIMK eMouseKeyID);
	_long GetDIMMoveState(DIMM eMouseMoveID);
	_bool GetDIMKeyDownState(DIMK eMouseKeyID);

public: /* For.PipeLine */
	void SetTransform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);
	_matrix GetTransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const;		
	_float4x4 GetTransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 GetTransformFloat4x4TP(CPipeLine::TRANSFORMSTATE eTransformState) const;
	const _float GetProjectionFar() const;
	const _float GetProjectionNear() const;
	_float4 GetCamPosition();

public: /* For.Light_Manager */
	const LIGHTDESC* GetLightDesc(_uint iIndex);
	CLight* AddLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);

public: /* For.Frustum */
	_bool isInFrustumWorldSpace(_fvector vWorldPos, float fRadius = 0.f);

public: /* For.Font_Manager */
	HRESULT AddFonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT RenderFonts(const _tchar* pFontTag, const _tchar* pTextm, _float2 vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		_float fAngle = 0.f, _float2 vOrigin = _float2(0.f, 0.f), _float2 vScale = _float2(1.f, 1.f));

public:  /* For.Target_Manager*/ 
	HRESULT AddRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor);
	HRESULT AddMRT(const _tchar* pMRTTag, const _tchar* pTargetTag);
	HRESULT BindRenderTargetSRV(const _tchar* pTargetTag, class CShader* pShader, const char* pConstantName);
	HRESULT ClearRenderTargetSRV(ID3D11DeviceContext* pContext);
	HRESULT BeginMRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDepthStencil = nullptr);
	HRESULT BeginOverwriteMRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDepthStencil = nullptr);
	HRESULT EndMRT(ID3D11DeviceContext* pContext);

#ifdef _DEBUG
	HRESULT AddDebugTarget(const _tchar* pTargetTag, _uint iCol, _uint iRow);
	HRESULT AddDebugMRT(const _tchar* pMRTTag);
#endif // _DEBUG

public: /* For.Sound Manager */
	HRESULT PlaySoundChannel(_tchar* pFileName, SOUND_CHANNEL eID, float volume);
	HRESULT StopSound(SOUND_CHANNEL eID);
	HRESULT PlayBGM(_tchar* pFileName, float volume);
	HRESULT StopBGM();
	HRESULT PlayOneShotEffect(_tchar* pFileName, float volume);
	HRESULT PlayEffect(_tchar* pFileName, float volume);
	HRESULT StopEffect();

#ifdef _DEBUG
	HRESULT RenderTool();
#endif // _DEBUG

private:
	CGraphicDevice* m_pGraphicDevice = nullptr;
	CInputDevice* m_pInputDevice = nullptr;
	CLevelManager* m_pLevelManager = nullptr;
	CObjectManager* m_pObjectManager = nullptr;
	CComponentManager* m_pComponentManager = nullptr;
	CTimerManager* m_pTimerManager = nullptr;
	CPipeLine* m_pPipeLine = nullptr;
	CLightManager* m_pLightManager = nullptr;
	CFrustum* m_pFrustum = nullptr;
	CFontManager* m_pFontManager = nullptr;
	CTargetManager* m_pTargetManager = nullptr;
	CSoundManager* m_pSoundManager = nullptr;
	CPicking* m_pPicking = nullptr;


#ifdef _DEBUG
	CImGuiManager* m_pImGuiManager = nullptr;
#endif // _DEBUG


public:
	static void ReleaseEngine();
	virtual void Free() override;
};

END