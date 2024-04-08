#pragma once
#include "Include.h"
#include "SpearMan.h"
#include "Daka.h"

class CSoldierMgr
{
public:
	CSoldierMgr();
	~CSoldierMgr();

public:
	void Initialize(const C2S_PACKET_GAMESTART& tGameStartPacket);
	void Release();

	const size_t GetSoldierVecSize() { return m_pSoldiers.size(); }
	void GetGameStartPacket(S2C_PACKET_GAMESTART& tOutGameStartPacket);

private:
	vector<class CSoldier*>m_pSoldiers;
};

