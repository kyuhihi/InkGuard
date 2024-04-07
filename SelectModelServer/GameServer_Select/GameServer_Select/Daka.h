#pragma once
#include "Soldier.h"

class CDaka : public CSoldier
{
public:
	CDaka();
	~CDaka();

public:
	virtual void Initialize() override;
	virtual void Release() override;
};

