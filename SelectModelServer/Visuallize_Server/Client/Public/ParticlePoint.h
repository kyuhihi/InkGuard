#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBufferPointInstance;

END

BEGIN(Client)

class CParticlePoint final : public CGameObject
{
private:
	CParticlePoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticlePoint(const CParticlePoint& rhs);
	virtual ~CParticlePoint() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint eRenderGroup) override;

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;	
	CVIBufferPointInstance*	m_pVIBufferCom = nullptr;
	
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */

public:
	static CParticlePoint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
private:

	CTransform* m_pTargetTransform = nullptr;
};

END