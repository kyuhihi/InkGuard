#include "stdafx.h"
#include "SpearMan.h"

CSpearMan::CSpearMan()
{
	Initialize();
}

CSpearMan::~CSpearMan()
{
	Release();
}

void CSpearMan::Initialize()
{
	m_tSoldierInfo.eSoldierType = SOLDIER_SPEAR;
}

void CSpearMan::Release()
{
}
