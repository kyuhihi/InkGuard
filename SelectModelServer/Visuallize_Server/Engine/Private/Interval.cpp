#include "..\Public\Interval.h"


CInterval::CInterval(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CInterval::CInterval(const CInterval& rhs)
	: CComponent(rhs)
{
}

HRESULT CInterval::InitializePrototype() 
{
	return S_OK;
}

HRESULT CInterval::Initialize(void* pArg) 
{
	return S_OK;
}

void CInterval::Update(_float fTimeDalta)
{
	auto iter = m_pIntervals.begin();

	while (iter != m_pIntervals.end()) 
	{
		if ((*iter).second->bDestroy) 
		{
			INTERVAL* DeleteObject = (*iter).second;
			iter = m_pIntervals.erase(iter);
			SafeDelete(DeleteObject);
		}
		else 
		{
			INTERVAL* intervalObject = (*iter).second;
			if (!intervalObject->bRunning)
			{
				iter++;
				continue;
			}

			intervalObject->fFrame += 60.f * fTimeDalta;

			if (intervalObject->fFrame > (60.f * intervalObject->fRepeatSecond))
			{
				intervalObject->fFrame = 0.f;

				if (intervalObject->eType == TYPE_ONE)
					intervalObject->bDestroy = true;

				if (intervalObject->pExcuteCallBack)
					intervalObject->pExcuteCallBack();
			}

			iter++;
		}
	}
}

HRESULT CInterval::AddInterval(const _tchar* pTag, function<void()> call, _float fStartTime)
{
	if (nullptr != FindInterval(pTag))
		return E_FAIL;

	INTERVAL* pInterval = new INTERVAL();
	if (nullptr == pInterval)
		return E_FAIL;

	pInterval->eType = TYPE_ONE;
	pInterval->pExcuteCallBack = call;
	pInterval->fRepeatSecond = fStartTime;

	m_pIntervals.emplace(pTag, pInterval);

	return S_OK;
}

HRESULT CInterval::AddRepeatInterval(const _tchar* pTag, std::function<void()> call, _float fRepeatSecond)
{
	if (nullptr != FindInterval(pTag))
		return E_FAIL;

	INTERVAL* pInterval = new INTERVAL();
	if (nullptr == pInterval)
		return E_FAIL;

	pInterval->eType = TYPE_REPEAT;
	pInterval->pExcuteCallBack = call;
	pInterval->fRepeatSecond = fRepeatSecond;

	m_pIntervals.emplace(pTag, pInterval);

	return S_OK;
}

HRESULT CInterval::RemoveInterval(const _tchar* tag) 
{
	auto iter = find_if(m_pIntervals.begin(), m_pIntervals.end(), CTagFinder(tag));

	if (iter == m_pIntervals.end())
		return E_FAIL;

	iter->second->bRunning = false;
	iter->second->bDestroy = true;

	return S_OK;
}

HRESULT CInterval::StopInterval(const _tchar* tag) 
{
	auto iter = find_if(m_pIntervals.begin(), m_pIntervals.end(), CTagFinder(tag));

	if (iter == m_pIntervals.end())
		return E_FAIL;

	iter->second->bRunning = false;

	return S_OK;
}

HRESULT CInterval::StopIntervalAll() 
{
	for (auto& pair : m_pIntervals) 
	{
		pair.second->bRunning = false;
	}

	return S_OK;
}

HRESULT CInterval::ClearInterval() 
{
	for (auto& pair : m_pIntervals) 
	{
		SafeDelete(pair.second);
	}

	m_pIntervals.clear();

	return S_OK;
}

CInterval::INTERVAL* CInterval::FindInterval(const _tchar* tag) 
{
	auto iter = find_if(m_pIntervals.begin(), m_pIntervals.end(), CTagFinder(tag));

	if (iter == m_pIntervals.end())
		return nullptr;

	return iter->second;
}

HRESULT CInterval::RestartInterval(const _tchar* tag) 
{
	auto iter = find_if(m_pIntervals.begin(), m_pIntervals.end(), CTagFinder(tag));

	if (iter == m_pIntervals.end())
		return E_FAIL;

	iter->second->bRunning = true;
	iter->second->fFrame = 0.f;

	return S_OK;

}

CInterval* CInterval::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
{
	CInterval* pInstance = new CInterval(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype())) 
	{
		MSG_BOX(TEXT("Failed To Created : Label"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent* CInterval::Clone(void* pArg) 
{
	CInterval* pInstance = new CInterval(*this);

	if (FAILED(pInstance->Initialize(pArg))) 
	{
		MSG_BOX(TEXT("Failed To Cloned : CInterval"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CInterval::Free() 
{
	__super::Free();

	for (auto& pair : m_pIntervals)
		SafeDelete(pair.second);
}