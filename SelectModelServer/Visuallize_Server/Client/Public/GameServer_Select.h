#pragma once
#include "Common.h"
#include "Network_Include.h"
#include "MatchMakingMgr.h"

class CGameServer_Select
{
public:
	CGameServer_Select();
	~CGameServer_Select();

private:
	void RemoveSocketInfo(int nIndex);
	bool AddSocketInfo(SOCKET sock);


public:
	void ServerLoop();
	void SetbDone();
	void Tidy();

private:
	int nTotalSockets = 0;
	CClient* ClientArray[FD_SETSIZE];// 총 64개의 소켓을 받을수있다.
	CMatchMakingMgr MatchMakingMgr;
	SOCKET listen_sock;

	bool m_bDone = false;
	mutex m_DoneMtx;
};

