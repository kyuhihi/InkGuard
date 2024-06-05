#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Player.h"

BEGIN(Client)

class CLevelGamePlay final : public CLevel
{
private:
	CLevelGamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevelGamePlay() = default;

public:
	virtual HRESULT Loading(_int eLevelID) override;
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void ReadyStarPos();

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SkyStar(const _tchar* pLayerTag);

public:
	static CLevelGamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

private:
	class CPlayer* m_pPlayer = nullptr;
	list<_float3>m_StarPosList;
};

END