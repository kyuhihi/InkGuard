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

			++m_iCurrentGameCnt;

			pSourClient->SetTeam(GAME_RED_TEAM);
			pSourClient->SetOtherClient(pDestClient);
			
			pDestClient->SetTeam(GAME_BLUE_TEAM);
			pDestClient->SetOtherClient(pSourClient);

			//pGameInstance->UpdateTimer(TEXT("Timer_Default"));

			//fTimeAcc += pGameInstance->GetTimeDelta(TEXT("Timer_Default"));

			//if (fTimeAcc >= 1.0f / g_iFPS/*0.0f*/)
			//{
			//	pGameInstance->UpdateTimer(TEXT("Timer_60"));
			//	pMainApp->Tick(pGameInstance->GetTimeDelta(TEXT("Timer_60")));
			//	pMainApp->Render();

			//	fTimeAcc = 0.f;
			//}
			bMatchSuccess = true;
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
