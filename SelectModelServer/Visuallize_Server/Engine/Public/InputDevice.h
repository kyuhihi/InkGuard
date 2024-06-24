#pragma once

#include "Base.h"

BEGIN(Engine)

class CInputDevice final : public CBase
{
	DECLARE_SINGLETON(CInputDevice)	
private:
	CInputDevice();
	virtual ~CInputDevice() = default;

public:
	HRESULT Initialize(HINSTANCE hInstance, HWND hWnd);
	void Update();

public:
	_char GetDIKState(_uchar eKeyID);

	_bool GetDIKDownState(_uchar eKeyID);
	_bool GetDIKUPState(_uchar eKeyID);

	_bool GetDIMKeyDownState(DIMK eMouseKeyID);

	_char GetDIMKeyState(DIMK eMouseKeyID) {
		return m_MouseState.rgbButtons[eMouseKeyID];
	}

	_long GetDIMMoveState(DIMM eMouseMoveID) {
		return ((_long*)&m_MouseState)[eMouseMoveID];
	}

private:
	LPDIRECTINPUT8				m_pInputSDK = nullptr;
	LPDIRECTINPUTDEVICE8		m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8		m_pMouse = nullptr;

private:	
	_char m_byKeyState[256] = { 0 };
	_bool m_keyDownState[256] = { false };
	_bool m_keyUpState[256] = { false };

	_bool m_mouseDown[DIMM::DIMM_END]{ false };
	DIMOUSESTATE m_MouseState;

public:
	virtual void Free();
};

END