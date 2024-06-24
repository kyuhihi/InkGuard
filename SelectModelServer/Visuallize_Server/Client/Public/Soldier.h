#pragma once
#include "Network_GameObject.h"
#include "Network_Include.h"

class CSoldier : public CNetwork_GameObject
{
public:
	enum SOLDIER_TYPE { SOLDIER_SPEAR, SOLDIER_DAKA, SOLDIER_END };
	enum TERRITORY_TYPE { TERRITORY_A, TERRITORY_B, TERRITORY_C, TERRITORY_END, };

public:
	struct SOLDIERINFO //서버에선 인덱스 번호가 추가되었음. 클라랑 구조체 구조가 다름.
	{
		SOLDIER_TYPE eSoldierType = SOLDIER_END;
		TERRITORY_TYPE eTargetTerritory = TERRITORY_END;
		int iSoldierIndex;
	};
	

public:// virtual
	virtual void Initialize() override;
	virtual void Release() override;

	virtual void SetSoldierInfo(const SOLDIER_TYPE eNewSoldierType, const TERRITORY_TYPE eNewTargetTerritoryType);
	virtual void SetSoldierInfo(const SOLDIERINFO tNewSoldierInfo) { m_tSoldierInfo = tNewSoldierInfo; };
	virtual void SetTargetTerritory(TERRITORY_TYPE eNewTargetTerritory) { m_tSoldierInfo.eTargetTerritory = eNewTargetTerritory; }
	
	virtual const SOLDIERINFO& GetSoldierInfo() { return m_tSoldierInfo; }

public:// normal
	void SetSoldierIndex(int iNewSoldierIndex) { m_tSoldierInfo.iSoldierIndex = iNewSoldierIndex; }
	void SetSoldierTransform(const S2C_PACKET_SOLDIER_TRANSFORM tRecvPacket);
	const S2C_PACKET_SOLDIER_TRANSFORM GetSoldierTransform() { return m_tSoldierTransform; };


protected:
	SOLDIERINFO m_tSoldierInfo;
	S2C_PACKET_SOLDIER_TRANSFORM m_tSoldierTransform;
};

