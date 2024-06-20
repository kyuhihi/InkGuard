#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Player.h"

BEGIN(Client)

class CCameraFree final : public CCamera
{
private:
	CCameraFree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraFree(const CCameraFree& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CCameraFree() = default;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CCameraFree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

public:
	void SetPlayer(class CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	const bool& IsFreeCamMode() const { return m_bFreeCam; }

private:
	class CPlayer* m_pPlayer = nullptr;
	bool m_bFreeCam = false;
};

END