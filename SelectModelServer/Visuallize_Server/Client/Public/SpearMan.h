#pragma once
#include "Soldier.h"

class CSpearMan : public CSoldier
{
public:
	CSpearMan();
	~CSpearMan();

public:
	virtual void Initialize() override;
	virtual void Release() override;


};

