#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CLight;
class CModel;
class CShader;
class CTexture;
class CCollider;
class CRenderer;
class CInterval;
class CTransform;
class CNavigation;

class CHierarchyNode;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint eRenderGroup);
	virtual HRESULT RenderLightDepth(CLight* pLight) override;

	void RenderIMGUI();
	void KeyInput(CTransform* pCamTransform,const _float fTimeDelta);

private:
	const _float WALK_SPEED;
	const _float RUN_SPEED;
	const _float PLAYER_CAM_DISTANCE;

private:
	enum ModelIndex {
		MODEL_MOUSE,
		MODEL_CLOSE_EYE,
		MODEL_ATTACK_EYE_AROUND,
		MODEL_BODY,
		MODEL_ATTACK_EYE,
		MODEL_KIND_EYE_AROUND,
		MODEL_KIND_EYE
	};

private:
	CShader* m_pShaderCom = nullptr;
				 
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CInterval* m_pIntervalComponent = nullptr;

private:
	HRESULT Ready_Components();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END