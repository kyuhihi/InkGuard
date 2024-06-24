#include "..\Public\TimerManager.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimerManager)

CTimerManager::CTimerManager()
{
}

_float CTimerManager::GetTimeDelta(const _tchar * pTimerTag)
{
	CTimer*		pTimer = FindTimer(pTimerTag);
	if (nullptr == pTimer)
		return 0.0f;

	return pTimer->GetTimeDelta();	
}

HRESULT CTimerManager::AddTimer(const _tchar * pTimerTag)
{
	if (nullptr != FindTimer(pTimerTag))
		return E_FAIL;

	CTimer*		pTimer = CTimer::Create();
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

void CTimerManager::Free()
{
	for (auto& Pair : m_Timers)
		SafeRelease(Pair.second);

	m_Timers.clear();
}
