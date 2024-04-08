#include "Soldier.h"

void CSoldier::Initialize()
{
}

void CSoldier::Release()
{
}

void CSoldier::SetSoldierInfo(const SOLDIER_TYPE eNewSoldierType, const TERRITORY_TYPE eNewTargetTerritoryType)
{
	m_tSoldierInfo.eSoldierType = eNewSoldierType;
	m_tSoldierInfo.eTargetTerritory = eNewTargetTerritoryType;
}
