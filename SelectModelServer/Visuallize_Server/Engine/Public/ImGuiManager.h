#pragma once

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CImGuiManager final : public CBase
{
	DECLARE_SINGLETON(CImGuiManager)

private:
	CImGuiManager();
	virtual ~CImGuiManager() = default;

public:
	HRESULT Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void FrameBegin();
	void Render();

public:
	static LRESULT ImGuiImplWin32WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	virtual void Free() override;
};
END