#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphicDevice final : public CBase
{
	DECLARE_SINGLETON(CGraphicDevice)

public:
	CGraphicDevice();
	virtual ~CGraphicDevice() = default;
public:
	HRESULT ReadyGraphicDevice(HWND hWnd, GRAPHICDESC::WINMODE WinMode, _uint iWinCX, _uint iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	HRESULT ClearBackBufferView(_float4 vClearColor);
	HRESULT ClearDepthStencilView();
	HRESULT Present();
private:	

	/* 메모리 할당. (정점버퍼, 인덱스버퍼, 텍스쳐로드) */
	ID3D11Device*			m_pDevice = nullptr;

	/* 바인딩작업. 기능실행 (정점버퍼를 SetStreamSource(), SetIndices(), SetTransform(), SetTexture() */
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	IDXGISwapChain*			m_pSwapChain = nullptr;	

	// ID3D11ShaderResourceView*
	ID3D11RenderTargetView*		m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;

private:
	HRESULT ReadySwapChain(HWND hWnd, GRAPHICDESC::WINMODE WinMode, _uint iWinCX, _uint iWinCY);
	HRESULT ReadyBackBufferRenderTargetView();
	HRESULT ReadyDepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	virtual void Free() override;
};

END