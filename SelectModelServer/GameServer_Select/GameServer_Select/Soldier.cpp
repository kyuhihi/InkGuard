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

void CSoldier::SetSoldierTransform(const S2C_PACKET_SOLDIER_TRANSFORM tRecvPacket)
{
	m_tSoldierTransform = tRecvPacket;
}
