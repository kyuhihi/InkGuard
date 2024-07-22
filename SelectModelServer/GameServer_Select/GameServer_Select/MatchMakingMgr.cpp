#include "MatchMakingMgr.h"
#include "TimerManager.h"

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

void CMatchMakingMgr::RemoveClient(CClient* pClient)
{
	const CClient::SOCKETINFO* pDestSocketInfo = pClient->GetSocketInfo();

	struct sockaddr_in pDestclientaddr;
	int Destaddrlen = sizeof(pDestclientaddr);
	getpeername(pDestSocketInfo->sock, (struct sockaddr*)&pDestclientaddr, &Destaddrlen);
	char Destaddr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &pDestclientaddr.sin_addr, Destaddr, sizeof(Destaddr));

	int iRemoveIndex = -1;
	for (int i =0; i < m_WaitingClientVec.size(); ++i)
	{
		const CClient::SOCKETINFO* tSrcSocketInfo = m_WaitingClientVec[i]->GetSocketInfo();
		struct sockaddr_in pSrcclientaddr;
		int Srcaddrlen = sizeof(pSrcclientaddr);
		getpeername(tSrcSocketInfo->sock, (struct sockaddr*)&pSrcclientaddr, &Srcaddrlen);
		char Srcaddr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &pSrcclientaddr.sin_addr, Srcaddr, sizeof(Srcaddr));

		if (ntohs(pDestclientaddr.sin_port) == ntohs(pSrcclientaddr.sin_port))
		{//삭제.
			iRemoveIndex = i;
			break;
		}
	}
	if(iRemoveIndex >= 0)
		m_WaitingClientVec.erase(m_WaitingClientVec.begin() + iRemoveIndex);
}

bool CMatchMakingMgr::CheckMatchMakingPossible()
{
	if (m_WaitingClientVec.size() < 2)					//적어도 한명보다는 많아야함.
		return false;

	bool bMatchSuccess = false;
	int i{ 0 }, j{ 0 };
	for (i = 0; i< m_WaitingClientVec.size(); ++i)
	{
		CClient* pSourClient = m_WaitingClientVec[i];
		for (j = 0; j < m_WaitingClientVec.size(); ++j)
		{
			CClient* pDestClient = m_WaitingClientVec[j];
			if (i == j)
				continue;

			CTimerManager* pTimerManager = CTimerManager::GetInstance();
			wstring strMatchTimer = pTimerManager->GetDefaultTimerName();
			strMatchTimer += to_wstring(m_iCurrentGameCnt);
			
			if (E_FAIL == pTimerManager->ReserveGameStart(strMatchTimer.c_str()))
				err_display("이거 타이머 못찾겠는데");

			pSourClient->SetTeam(GAME_RED_TEAM);
			pSourClient->SetOtherClient(pDestClient);
			pSourClient->SetTimerTag(strMatchTimer.c_str());
			
			pDestClient->SetTeam(GAME_BLUE_TEAM);
			pDestClient->SetOtherClient(pSourClient);
			pDestClient->SetTimerTag(strMatchTimer.c_str());


			bMatchSuccess = true;
			++m_iCurrentGameCnt;
			break;
		}
		if (bMatchSuccess) {
			break;
		}
	}

	m_WaitingClientVec.erase(m_WaitingClientVec.begin() + i);
	m_WaitingClientVec.erase(m_WaitingClientVec.begin() + (j - 1));


	CheckMatchMakingPossible(); //재귀호출.

	return true;
}
