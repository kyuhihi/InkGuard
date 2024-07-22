#pragma once

#include "Include.h"

class CTimer final 
{
public:
	CTimer();
	~CTimer();

public:
	float GetGameTime(void);
	void SetGameTime(const float fNewTime) { m_bGameStart = false; m_fGameTime = fNewTime; };

public:
	HRESULT Initialize();
	void Update(void);


private:
	LARGE_INTEGER		m_CurrentTime;
	LARGE_INTEGER		m_OldTime;
	LARGE_INTEGER		m_OriginTime;
	LARGE_INTEGER		m_CpuTick;
	float				m_fTimeDelta;

	float				m_fGameTime = TOTAL_GAME_TIME;
	bool				m_bGameStart = false;
};
