#pragma once

#include "Client_Defines.h"
#include "Camera.h"

/* Playable Chase Camera */
BEGIN(Client)

class CChaseCamera final : public CCamera
{
private:
	CChaseCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChaseCamera(const CChaseCamera& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CChaseCamera() = default;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void FreeModeTick(_float fTimeDelta);

public:
	void Chase(CTransform* pTransform) {
		m_pChaseTransform = pTransform;
	}

private:
	CTransform* m_pChaseTransform;

public:
	static CChaseCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END

