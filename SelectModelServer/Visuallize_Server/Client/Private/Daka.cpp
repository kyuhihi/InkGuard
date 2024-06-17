#include "stdafx.h"
#include "Daka.h"

CDaka::CDaka()
{
	Initialize();
}

CDaka::~CDaka()
{
	Release();
}

void CDaka::Initialize()
{
	m_tSoldierInfo.eSoldierType = SOLDIER_DAKA;
}

void CDaka::Release()
{
}
