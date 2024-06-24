#pragma once

#include "Base.h"

BEGIN(Engine)
class CLight;
class CLightManager final : public CBase
{
	DECLARE_SINGLETON(CLightManager)
public:
	CLightManager();
	virtual ~CLightManager() = default;

public:
	CLight* AddLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBufferRect* pVIBuffer);
	const LIGHTDESC* GetLightDesc(_uint iIndex);

public:
	const _uint GetNumLights() const {
		return static_cast<_uint>(m_lights.size());
	}

	CLight* GetLight(_uint index) const {
		if (index >= m_lights.size())
			return nullptr;

		auto iter = m_lights.begin();
		for (size_t i = 0; i < index; ++i) {
			iter++;
		}

		return *iter;
	}

private:
	using LIGHTS = list<CLight*>;
	LIGHTS m_lights;

public:
	virtual void Free() override;
};

END