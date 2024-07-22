#include "Timer.h"
#include "TimerManager.h"

CTimerManager* CTimerManager::m_pInstance = nullptr;

CTimerManager::CTimerManager()
	
{
}

CTimerManager::~CTimerManager()
{
	for (auto& Pair : m_Timers) {
		delete Pair.second;
		Pair.second = nullptr;
	}

	m_Timers.clear();
}

_float CTimerManager::GetTimeDelta(const _tchar * pTimerTag)
{
	CTimer*		pTimer = FindTimer(pTimerTag);
	if (nullptr == pTimer)
		return 0.0f;

	return pTimer->GetTimeDelta();	
}

_float CTimerManager::GetGameTime(const _tchar* pTimerTag)
{
	return _float();
}

HRESULT CTimerManager::AddTimer(const _tchar * pTimerTag)
{
	if (nullptr != FindTimer(pTimerTag))
		return E_FAIL;

	CTimer*		pTimer = new CTimer;
	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.emplace(pTimerTag, pTimer);

	return S_OK;
}

HRESULT CTimerManager::UpdateTimer(const _tchar * pTimerTag)
{
	CTimer*		pTimer = FindTimer(pTimerTag);
	if (nullptr == pTimer)
		return E_FAIL;

	pTimer->Update();	

	return S_OK;
}

CTimer * CTimerManager::FindTimer(const _tchar * pTimerTag)
{
	auto	iter = find_if(m_Timers.begin(), m_Timers.end(), CTagFinder(pTimerTag));

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}