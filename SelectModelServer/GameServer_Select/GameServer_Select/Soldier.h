#pragma once
#include "GameObject.h"
#include "Include.h"

class CSoldier : public CGameObject
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

public:
	virtual void Initialize() override;
	virtual void Release() override;

protected:
	SOLDIERINFO m_tSoldierInfo;
};

