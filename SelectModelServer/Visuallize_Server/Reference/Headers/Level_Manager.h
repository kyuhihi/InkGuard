#pragma once

#include "Base.h"

/* ���� ��������� ������ �����͸� ������ �ִ´�. */
/* ������ ƽ, ������ ����ȣ���Ѵ�. */
/* ��[����ü�ÿ� �������� ����,. ���ο�� ��ü�Ѵ�. */

BEGIN(Engine)
class CLevel;
class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	/* �������� ����. ���ο�� ����. */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
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