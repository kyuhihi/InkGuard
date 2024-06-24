#pragma once
#include "OBB.h"
#include "AABB.h"
#include "Model.h"
#include "Shader.h"
#include "Sphere.h"
#include "Texture.h"
#include "Renderer.h"
#include "Interval.h"
#include "Transform.h"
#include "Navigation.h"
#include "VIBufferRect.h"
#include "VIBufferCube.h"
#include "VIBufferTerrain.h"
#include "VIBufferRectInstance.h"
#include "VIBufferPointInstance.h"


BEGIN(Engine)

class CComponentManager final : public CBase
{
	DECLARE_SINGLETON(CComponentManager)
private:
	CComponentManager();
	virtual ~CComponentManager() = default;

public:
	HRESULT ReserveContainer(_uint iNumLevels);
	HRESULT AddPrototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* CloneComponent(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);

	void Clear(_uint iLevelIndex);

private:
	_uint m_iNumLevels = 0;

	using PROTOTYPES = map<const _tchar*, class CComponent*>;
	PROTOTYPES* m_pPrototypes = nullptr;

private:
	class CComponent* FindComponent(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END