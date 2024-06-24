#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CTerritory final : public CGameObject
{
public:
	enum SHAPE { SHAPE_CIRCLE, SHAPE_RECT, SHAPE_END };
	enum ROTATE { ROTATE_LT, ROTATE_LB, ROTATE_RT, ROTATE_RB, ROTATE_END };


private:
	CTerritory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerritory(const CTerritory& rhs);
	virtual ~CTerritory() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint eRenderGroup) override;
	virtual HRESULT RenderLightDepth(CLight* pLight) override;

	void RenderIMGUI();
private:
	void Modify_Transform();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pCurrentModel = nullptr;
	CModel*					m_pModelCom_Rect = nullptr;
	CModel*					m_pModelCom_Circle = nullptr;

	const _float m_Territory_Radius_Circle = 11.52f;
	const _float m_Territory_Radius_Rect = 7.68f;

	int m_iCurrentSelected = 0;
	
private:
	HRESULT Ready_Components();

public:
	static CTerritory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END