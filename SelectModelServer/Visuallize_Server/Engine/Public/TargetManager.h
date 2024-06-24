#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget;
class CTargetManager final : public CBase
{
	DECLARE_SINGLETON(CTargetManager)
public:
	CTargetManager();
	virtual ~CTargetManager() = default;

public:	
	HRESULT AddRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor);
	HRESULT AddMRT(const _tchar* pMRTTag, const _tchar* pTargetTag);

public:
	HRESULT BindSRV(const _tchar* pTargetTag, class CShader* pShader, const char* pConstantName);
	HRESULT BeginMRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDepthStencil = nullptr);
	HRESULT BeginOverwriteMRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDepthStencil = nullptr);
	HRESULT EndMRT(ID3D11DeviceContext* pContext);
	HRESULT ClearSRV(ID3D11DeviceContext* pContext);

	ID3D11DepthStencilView* CreateDepthStencil(ID3D11Device* pDevice, _uint iWinCX, _uint iWinCY);
	D3D11_VIEWPORT CreateViewPort(_uint iWinCX, _uint iWinCY);

#ifdef _DEBUG
public:	
	HRESULT InitializeDebug(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
public:
	HRESULT AddDebugTarget(const _tchar* pTargetTag, _uint iCol, _uint iRow);
	HRESULT AddDebugMRT(const _tchar* pMRTTag);
	HRESULT RenderDebug(class CVIBuffer* pVIBuffer, class CShader* pShader);
#endif // _DEBUG

private:
	using RENDERTARGETS = map<const _tchar*, CRenderTarget*>;
	RENDERTARGETS m_RenderTargets;

private:
	using MRTS = map<const _tchar*, list< CRenderTarget*>>;
	MRTS m_MRTs;

#ifdef _DEBUG
private:
	list<list<CRenderTarget*>*> m_DebugMRTs;
#endif // _DEBUG

private:
	ID3D11RenderTargetView* m_pOldRenderTargets[8] = { nullptr };
	ID3D11DepthStencilView* m_pOldDepthStencil = nullptr;

private:
	class CRenderTarget* Find_RenderTarget(const _tchar* pTargetTag);
	list<CRenderTarget*>* FindMRT(const _tchar* pMRTTag);

public:
	virtual void Free() override;
};

END