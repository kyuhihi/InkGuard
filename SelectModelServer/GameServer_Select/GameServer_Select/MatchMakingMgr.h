#pragma once
#include "Include.h"
#include "Client.h"

class CMatchMakingMgr
{
public:
	CMatchMakingMgr();
	~CMatchMakingMgr();

public:
	void AddClient(CClient* pClient);

private:
	bool CheckMatchMakingPossible();

private:
	vector<CClient*> m_WaitingClientVec;
};

