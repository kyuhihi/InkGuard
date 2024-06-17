#include "stdafx.h"
#include "SoldierMgr.h"


CSoldierMgr::CSoldierMgr()
{
}

CSoldierMgr::~CSoldierMgr()
{
	Release();
}

void CSoldierMgr::Initialize(const C2S_PACKET_GAMESTART& tGameStartPacket)// 받은 패킷으로 새로운 솔져들 new로 할당해줌.
{
	for (int i = 0; i < SOLDIER_MAX_CNT; ++i)
	{
		CSoldier::SOLDIER_TYPE eSoldierType = (CSoldier::SOLDIER_TYPE)tGameStartPacket.cSoldierInfo[i];
		CSoldier* pNewSoldier = nullptr;
		switch (eSoldierType)
		{
		case CSoldier::SOLDIER_SPEAR: {
			pNewSoldier = new CSpearMan();
			break;
		}
		case CSoldier::SOLDIER_DAKA: {
			pNewSoldier = new CDaka();
			break;
		}
		case CSoldier::SOLDIER_END: {
			break;
		}
		default:
			break;
		}
		pNewSoldier->SetTargetTerritory((CSoldier::TERRITORY_TYPE)tGameStartPacket.cTargetTerritory[i]);
		pNewSoldier->SetSoldierIndex(i);

		m_pSoldiers.push_back(pNewSoldier);
	}
}

void CSoldierMgr::Release()
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

void CSoldierMgr::GetGameStartPacket(S2C_PACKET_GAMESTART& tOutGameStartPacket)
{
	for (int i = 0; i < SOLDIER_MAX_CNT; ++i) {
		auto& SoldierInfo = m_pSoldiers[i]->GetSoldierInfo();
		tOutGameStartPacket.cOtherSoldierInfo[i] = SoldierInfo.eSoldierType;
		tOutGameStartPacket.cOtherTargetTerritory[i] = SoldierInfo.eTargetTerritory;
	}
	return;
}

void CSoldierMgr::SetGameStartPacket(const C2S_PACKET_GAMESTART& tGameStartPacket)
{
	for (int i = 0; i < SOLDIER_MAX_CNT; i++) 
	{
		m_pSoldiers[i]->SetTargetTerritory((CSoldier::TERRITORY_TYPE)tGameStartPacket.cTargetTerritory[i]);
		m_pSoldiers[i]->SetSoldierIndex(i);
	}
}

void CSoldierMgr::SetSoldiersPacket(const S2C_PACKET_SOLDIER_TRANSFORM* pRecvPacket, const int iStartIndex)
{
	int iEndIndex = 4; //디폴트로 0~4 임.
	if (iStartIndex == 4)
	{//0,1,2,3		//4,5,6,7,8
		iEndIndex = SOLDIER_MAX_CNT;
	}

	for (int i = iStartIndex; i < iEndIndex; i++)
	{
		m_pSoldiers[i]->SetSoldierTransform(pRecvPacket[i]);
	}

	

}

void CSoldierMgr::GetSoldiersPacket(S2C_PACKET_SOLDIER_TRANSFORM* pSendPacket)
{
	for(int i=0; i< SOLDIER_MAX_CNT; i++)
	{
		pSendPacket[i] = m_pSoldiers[i]->GetSoldierTransform();
	}
}
