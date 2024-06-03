#pragma once

#include "Timer.h"

BEGIN(Engine)

class CTimerManager final : public CBase
{
	DECLARE_SINGLETON(CTimerManager);
private:
	CTimerManager();
	virtual ~CTimerManager() = default;

public:
	_float GetTimeDelta(const _tchar* pTimerTag);

public:
	HRESULT AddTimer(const _tchar* pTimerTag);
	HRESULT UpdateTimer(const _tchar* pTimerTag);

private:
	using TIMERS = map<const _tchar*, class CTimer*>;
	TIMERS m_Timers;

private:
	class CTimer* FindTimer(const _tchar* pTimerTag);

public:	
	virtual void Free() override;
};

END