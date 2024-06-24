#pragma once

#include "Base.h"

/* 현재 보여줘야할 레벨의 퐁니터를 가지고 있는다. */
/* 레벨의 틱, 렌더를 무한호출한다. */
/* 레[벨교체시에 기존레벨 삭제,. 새로운로 교체한다. */

BEGIN(Engine)
class CLevel;
class CLevelManager final : public CBase
{
	DECLARE_SINGLETON(CLevelManager)
private:
	CLevelManager();
	virtual ~CLevelManager() = default;

public:
	/* 기존레벨 삭제. 새로운레벨 셋팅. */
	HRESULT OpenLevel(_uint iLevelIndex, class CLevel* pNewLevel);
	HRESULT SceneSwap();

	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	HRESULT Render();

	CLevel* GetLevel();

	void LevelOpenReady(_bool ready) { m_bOpenReady = ready; };



private:
	CLevel* m_pCurrentLevel = nullptr;
	_uint m_iCurrentLevelIndex = 0;

	CLevel* m_pReservationLevel;
	_uint m_iReservationLevelIndex;
	_bool m_bOpenReady;


public:
	virtual void Free() override;
};

END