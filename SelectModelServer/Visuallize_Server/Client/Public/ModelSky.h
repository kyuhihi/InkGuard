#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CModelSky final : public CGameObject
{
private:
	enum SKY_PASS { PASS_DAY, PASS_NIGHT };
private:
	CModelSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModelSky(const CModelSky& rhs);
	virtual ~CModelSky() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint eRenderGroup) override;

public:
	void SetNight();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;	
	CModel*					m_pModelCom = nullptr;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources();
	_float m_fSkyUV{ 0.f };
	_float m_fBlueColor[4];//0 번패스
	const _float m_fDayBlueColor[4]{ 0.1875f, 0.6601f, 0.8671f, 1.f };//0번 패스
	const _float m_fNightBlueColor[4]{ 0.f, 0.f, 0.2f, 1.f };//1번 패스
	SKY_PASS m_eSkyPass{ PASS_DAY };

public:
	static CModelSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END