#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBufferCube;
END

BEGIN(Client)

class CGround final : public CGameObject
{
private:
	CGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGround(const CGround& rhs);
	virtual ~CGround() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint eRenderGroup) override;


private:
	CShader*		m_pShaderCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CVIBufferCube* m_pVIBufferCom = nullptr;
	CTexture* m_pTextureCom = nullptr;

private:
	HRESULT Ready_Components();
	const _float GROUND_SIZE;


public:
	static CGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END