#include "..\Public\InputDevice.h"

IMPLEMENT_SINGLETON(CInputDevice)

CInputDevice::CInputDevice()
{
}

HRESULT CInputDevice::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	/* m_pInputSDK */
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;
	if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pKeyboard->Acquire()))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;


	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;
	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return S_OK;
}

void CInputDevice::Update()
{
	m_pKeyboard->GetDeviceState(256, m_byKeyState);

	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);

	for (int i = 0; i < 256; ++i) 
	{
		if (m_keyDownState[i] && !(m_byKeyState[i] & 0x80)) 
		{
			m_keyDownState[i] = false;
			m_keyUpState[i] = true;
		}
		else if (m_keyUpState[i]) 
		{
			m_keyUpState[i] = false;
		}
	}

	for (int i = 0; i < DIMM::DIMM_END; ++i) 
	{
		if (m_mouseDown[i] && !(m_MouseState.rgbButtons[i] & 0x80)) 
		{
			m_mouseDown[i] = false;
		}
	}
}

_char CInputDevice::GetDIKState(_uchar eKeyID) 
{
	m_keyDownState[eKeyID] = true;

	return m_byKeyState[eKeyID];
}

_bool CInputDevice::GetDIKDownState(_uchar eKeyID) 
{
	m_pKeyboard->GetDeviceState(256, m_byKeyState);

	if (!m_keyDownState[eKeyID] && m_byKeyState[eKeyID] & 0x80) {
		m_keyDownState[eKeyID] = true;

		return true;
	}

	return false;
}

_bool CInputDevice::GetDIKUPState(_uchar eKeyID) 
{
	m_pKeyboard->GetDeviceState(256, m_byKeyState);

	if (m_keyUpState[eKeyID]) {
		m_keyUpState[eKeyID] = false;
		m_keyDownState[eKeyID] = false;

		return true;
	}

	return false;
}

_bool CInputDevice::GetDIMKeyDownState(DIMK eMouseKeyID) 
{
	if (!m_mouseDown[eMouseKeyID] && m_MouseState.rgbButtons[eMouseKeyID] & 0x80) {
		m_mouseDown[eMouseKeyID] = true;

		return true;
	}

	return false;
}

void CInputDevice::Free()
{
	SafeRelease(m_pKeyboard);
	SafeRelease(m_pMouse);

	SafeRelease(m_pInputSDK);
}
