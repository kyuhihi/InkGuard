#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)
public:
	CPicking();
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinCX, _uint iWinCY);
	void Tick(); /* 월드스페이스 상의 마우스레이, 레이포스를 구한다. */
	_vector Compute_LocalRayInfo(_bool bPos, class CTransform * pTransform);
private:

	HWND				m_hWnd;
	_uint				m_iWinCX, m_iWinCY;

	_float3				m_vRayDir;
	_float3				m_vRayPos;
public:
	virtual void Free() override;
};

END