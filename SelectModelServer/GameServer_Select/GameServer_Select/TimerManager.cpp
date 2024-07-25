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

_float CTimerManager::GetGameTime(const _tchar* pTimerTag)
{
	CTimer* pTimer = FindTimer(pTimerTag);
	if (nullptr == pTimer) {
		err_display("Timer못찾겠는데?\n");
		return -1.f;
	}

	return pTimer->GetGameTime();
}

HRESULT CTimerManager::AddTimer(const _tchar * pTimerTag)
{
	if (nullptr != FindTimer(pTimerTag))
		return E_FAIL;

	CTimer*		pTimer = new CTimer;
	if (nullptr == pTimer)
		return E_FAIL;

	size_t length = wcslen(pTimerTag) + 1;

	// Allocate new memory for the string copy.
	_tchar* pNewTimerTag = new _tchar[length];

	// Copy the string to the newly allocated memory.
	wcscpy(pNewTimerTag, pTimerTag);

	m_Timers.emplace(pNewTimerTag, pTimer);

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

HRESULT CTimerManager::ReserveGameStart(const _tchar* pTimerTag)
{
	CTimer* pTimer = FindTimer(pTimerTag);
	if (nullptr == pTimer)
		return E_FAIL;

	pTimer->SetGameTime(TOTAL_GAME_TIME);
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