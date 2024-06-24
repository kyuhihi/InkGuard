#pragma once

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

extern bool g_bDebug;
extern bool g_bKeyInput;
extern int	g_iFPS;

using namespace Client;
