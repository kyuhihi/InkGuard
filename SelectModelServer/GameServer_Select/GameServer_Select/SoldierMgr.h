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

	void SetGameStartPacket(const C2S_PACKET_GAMESTART& tGameStartPacket);
	
	void SetSoldiersPacket(const S2C_PACKET_SOLDIER_TRANSFORM* pRecvPacket,const int iStartIndex);
	void GetSoldiersPacket(S2C_PACKET_SOLDIER_TRANSFORM* pSendPacket);

private:
	vector<class CSoldier*>m_pSoldiers; // 9
};

