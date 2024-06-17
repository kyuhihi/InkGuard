#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBufferRect;
END

BEGIN(Client)

class CSkyStar final : public CGameObject
{
private:
	enum AURORA_SEQUENCE_COLOR { AURORA_YELLOW, AURORA_GREEN, AURORA_ORANGE ,AURORA_END};

private:
	CSkyStar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkyStar(const CSkyStar& rhs);
	virtual ~CSkyStar() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint eRenderGroup) override;

private:
	void AuroraColorChangeTick(_float fTimeDelta);
	void CheckAuroraFinish();
	_uint CheckArraySize(_uint iSequence);

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBufferRect*			m_pVIBufferCom = nullptr;

private:
	HRESULT Ready_Components();
	_float m_fSequenceLerp{ 0.f };
	_vector m_vAuroraSequence[AURORA_END];
	_float4 m_vCurAuroraColor = { 256.f,256.f,256.f,256.f };
	_uint m_iCurAuroraSequence{ 0 };
	_float m_fDisappearMaxTime{ 1.f };
	_float m_fAlphaTime{ 1.f };
	_bool m_bCulling{ false };

	_float3 m_vStarPosition = _float3();

public:
	static CSkyStar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END