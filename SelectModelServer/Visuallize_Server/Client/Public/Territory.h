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

	struct DebugTerritoryStruct
	{
		SHAPE eNewShape = SHAPE_END;
		_float3 vPosition;
	};

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


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	DebugTerritoryStruct m_tInfo;

private:
	HRESULT Ready_Components();

public:
	static CTerritory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END