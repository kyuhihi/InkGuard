#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
public:
	CLayer();
	virtual ~CLayer() = default;
public:
	class CComponent* GetComponentPtr(const _tchar* pComponentTag, _uint iLayerIndex);

public:
	HRESULT AddGameObject(class CGameObject* pGameObject);

	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

private:
	using GAMEOBJECTS = list<class CGameObject*>;
	GAMEOBJECTS m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END