#pragma once

#include "Timer.h"


class CTimerManager final
{
#pragma region Declare Singleton
public:
	static CTimerManager* GetInstance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CTimerManager;	//�ν��Ͻ��� ���ٸ� �ϳ� �����մϴ�.

		return m_pInstance;
	}
	static void DestroyInstance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	CTimerManager();
	~CTimerManager();

private:
	static CTimerManager* m_pInstance;
#pragma endregion

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

};