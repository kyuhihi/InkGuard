#include "MatchMakingMgr.h"

CMatchMakingMgr::CMatchMakingMgr()
{
}

CMatchMakingMgr::~CMatchMakingMgr()
{
	for (auto& pClient : m_WaitingClientVec)
		pClient = nullptr;

	m_WaitingClientVec.clear();
}

void CMatchMakingMgr::AddClient(CClient* pClient)
{
	m_WaitingClientVec.push_back(pClient);
	CheckMatchMakingPossible();
}

bool CMatchMakingMgr::CheckMatchMakingPossible()
{
	if (m_WaitingClientVec.size() < 2)					//적어도 한명보다는 많아야함.
		return false;

	bool bMatchSuccess = false;
	int i{ 0 }, j{ 0 };
	for (int i = 0; i< m_WaitingClientVec.size(); ++i)
	{
		CClient* pSourClient = m_WaitingClientVec[i];
		for (int j = 0; j < m_WaitingClientVec.size(); ++j)
		{
			CClient* pDestClient = m_WaitingClientVec[j];
			if (i == j)
				continue;

			pSourClient->SetOtherClient(pDestClient);
			pDestClient->SetOtherClient(pSourClient);
			bMatchSuccess = true;
			break;
		}
		if (bMatchSuccess)
			break;
	}

	m_WaitingClientVec.erase(m_WaitingClientVec.begin() + i);
	m_WaitingClientVec.erase(m_WaitingClientVec.begin() + (j - 1));


	CheckMatchMakingPossible();

	return true;
}
