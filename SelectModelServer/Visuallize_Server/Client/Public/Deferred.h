#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBufferRect;
END

BEGIN(Client)

class CDeferred final : public CGameObject
{
public:
	enum TECHNIQUE {
		TECHNIQUE_DEFAULT,
		TECHNIQUE_END
	};

	enum DEFAULT_TECHNIQUE_PASS {
		DEFAULT_BLEND,
		DEFAULT_END
	};

private:
	CDeferred(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDeferred(const CDeferred& rhs);
	virtual ~CDeferred() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint eRenderGroup) override;

private:
	CShader* m_pShaderComponent;
	CRenderer* m_pRendererComponent;
	CVIBufferRect* m_pVIBufferComponent;

private:
	HRESULT ReadyComponents();

private:
	_float4x4 m_viewMatrix;
	_float4x4 m_projMatrix;

public:
	static CDeferred* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
