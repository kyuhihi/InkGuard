#pragma once

/* 특정 레벨에 사용되는 객체들응ㄹ 모아서 보관해두낟. */
/* 보관하고 잇는 개체들의 Tick를 호출해준다. */

#include "Base.h"

BEGIN(Engine)

class CObjectManager final : public CBase
{
	DECLARE_SINGLETON(CObjectManager)
private:
	CObjectManager();
	virtual ~CObjectManager() = default;

public:
	class CComponent* GetComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iLayerIndex);

public:
	HRESULT ReserveContainer(_uint iNumLevels);
	class CGameObject* CloneGameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	class CGameObject* CloneGameObject(const _tchar* pPrototypeTag, _uint iLevelIndex, void* pArg = nullptr);
	HRESULT AddPrototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	CGameObject* AddGameObjectToLayer(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr);

public:
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
	

private: /* 원형객체들을 보관할 커넽이너ㅣ */
	using PROTOTYPES = map<const _tchar*, class CGameObject*>;
	PROTOTYPES m_Prototypes;


private: /* 사본객체들을 보관할 커넽이너ㅣ */
	_uint m_iNumLevels = 0;
	using LAYERS = map<const _tchar*, class CLayer*>;
	LAYERS* m_pLayers = nullptr;

private:
	class CGameObject* FindPrototype(const _tchar* pPrototypeTag);
	class CLayer* FindLayer(_uint iLevelIndex, const _tchar* pLayerTag);
		
public:
	virtual void Free() override;
};

END