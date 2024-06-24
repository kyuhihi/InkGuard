#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Client)
class CShaderCam final : public CCamera
{
public:
	enum SHADERCAM { SHADER_RIM_ANIM, SHADER_END };
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_TEST, LEVEL_TOOL, LEVEL_KYU, LEVEL_SHADERTOOL, LEVEL_ANIMATION, LEVEL_END };

private:
	CShaderCam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShaderCam(const CShaderCam& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CShaderCam() = default;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<CShader*> m_vShaderCom;
	typedef vector<CShader*> SHADERS;

	_float m_fCTP = 5.f;

private:
	HRESULT ReadyShaderComponents();
	void KeyInput(_float fTimeDelta);

public:
	static CShaderCam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END