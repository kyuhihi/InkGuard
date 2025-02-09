#pragma once
#include "Network_Include.h"
#include "Network_Client.h"

class CMatchMakingMgr
{
public:
	CMatchMakingMgr();
	~CMatchMakingMgr();

public:
	void AddClient(CClient* pClient);
	void RemoveClient(CClient* pClient);

private:
	bool CheckMatchMakingPossible();

private:
	vector<CClient*> m_WaitingClientVec;
};

