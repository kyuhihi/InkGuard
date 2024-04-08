#pragma once
#include "Include.h"
#include "Packet.h"
#include "Player.h"
#include "SpearMan.h"
#include "Daka.h"

class SoldierMgr
{
public:
	SoldierMgr();
	~SoldierMgr();

public:
	void Initialize(const C2S_PACKET_GAMESTART* tGameStartPacket);
	void Release();

private:
	vector<class CSoldier*>m_pSoldiers;
};

