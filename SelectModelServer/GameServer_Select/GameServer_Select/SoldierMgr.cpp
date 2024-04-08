#include "SoldierMgr.h"

SoldierMgr::SoldierMgr()
{
}

SoldierMgr::~SoldierMgr()
{
	Release();
}

void SoldierMgr::Initialize(const C2S_PACKET_GAMESTART* tGameStartPacket)
{
	for (int i = 0; i < SOLDIER_MAX_CNT; ++i)
	{
		SOLDIER_
		tGameStartPacket->cOtherSoldierInfo[i];
	}
}

void SoldierMgr::Release()
{
	if (!m_pSoldiers.empty())
	{
		for (auto& iter : m_pSoldiers)
		{
			delete iter;
			iter = nullptr;
		}
		m_pSoldiers.clear();
	}

}
