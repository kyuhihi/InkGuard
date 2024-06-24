#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	using NAVIGATIONDESC = struct tagNavigationDesc
	{
		_int iCurrentIndex = -1;
	};
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT InitializePrototype(const _tchar* pNavigationDataFilePath);
	virtual HRESULT Initialize(void* pArg);

public:
	_bool isMove(_fvector vPosition);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

private:
	using CELLS = vector<class CCell*>;
	CELLS m_Cells;

	NAVIGATIONDESC m_NavigationDesc;

private:
	HRESULT ReadyNeighbor();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath);
	CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END 