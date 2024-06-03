#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevelLoading final : public CLevel
{
private:
	CLevelLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevelLoading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevel);
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT CreateIDToLevel(LEVEL eNextLevel);

private:
	LEVEL m_eNextLevel = LEVEL_END;
	CLevel* m_pNextLevel;
	class CLoader* m_pLoader = nullptr;

public:
	static CLevelLoading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END