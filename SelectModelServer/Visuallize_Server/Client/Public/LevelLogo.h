#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevelLogo final : public CLevel
{
private:
	CLevelLogo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevelLogo() = default;

public:
	virtual HRESULT Loading(_int eLevelID) override;
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);

public:
	static CLevelLogo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END