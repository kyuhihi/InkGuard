#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CLevel;
END

BEGIN(Client)
class CLoader final : public CBase
{
public:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	LEVEL GetNextLevelID() const {
		return m_eNextLevelID;
	}

	CRITICAL_SECTION GetCS() const {
		return m_CriticalSection;
	}

public:
	HRESULT Initialize(LEVEL eNextLevelID, CLevel* pNextLevel);

public:
	HRESULT LoadingLevel();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

private:
	HANDLE m_hThread = 0;
	CRITICAL_SECTION m_CriticalSection;
	LEVEL m_eNextLevelID = LEVEL_END;

private:
	CLevel* m_pNextLevel;

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID, CLevel* pNextLevel);
	virtual void Free() override;
};

END