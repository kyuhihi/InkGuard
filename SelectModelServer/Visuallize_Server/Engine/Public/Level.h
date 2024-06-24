#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Loading(_int eLevelID);
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	const _bool GetLoadFinished() const {
		return m_isLoadFinished;
	}

	const _tchar* GetLoadingText() const {
		return m_szLoadingText;
	}

protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	_tchar m_szLoadingText[MAX_PATH] = TEXT("");

private:
	_bool m_isLoadFinished;
	_int m_eLevelID;

public:
	virtual void Free() override;
};

END