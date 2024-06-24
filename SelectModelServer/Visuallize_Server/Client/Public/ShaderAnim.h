#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CHierarchyNode;
END

BEGIN(Client)

class CShaderAnim final : public CGameObject
{
public:
	enum SHADERMODEL { SHADER_ANIM, SHADER_RIM_ANIM, SHADER_CARTOON_ANIM, SHADER_END };


private:
	CShaderAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShaderAnim(const CShaderAnim& rhs);
	virtual ~CShaderAnim() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint eRenderGroup);

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;

private:
	vector<CShader*> m_vShaderCom;
	typedef vector<CShader*> SHADERS;

	_uint m_iIndexForNullArg = SHADER_ANIM;
	_uint* m_iShaderIndex = nullptr;

private:
	HRESULT Ready_Components();
	void KeyInput(_float fTimeDelta);

public:
	static CShaderAnim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END