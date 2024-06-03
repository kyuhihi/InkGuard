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

class CTestEffect final : public CGameObject
{
public:
	enum eEffectType
	{
		CUSTOM, CONVERAGE, SPREAD, TYPE_END
	};

	struct EffectDesc
	{
		_float3 fDir;
		_float3 fPos;
		wchar_t* strTextureName;
		_float fDuration;
		_float fSpeed;
		_float fRotateSpeed;
		_float fMaxDistance;
		_float fMinDistance;
		eEffectType eType;
	};

private:
	CTestEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestEffect(const CTestEffect& rhs);
	virtual ~CTestEffect() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint eRenderGroup) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBufferRect*			m_pVIBufferCom = nullptr;

private:
	EffectDesc				m_eDesc;
	_float					m_fFrame = 0.f;

private:
	HRESULT Ready_Components();

public:
	static CTestEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END