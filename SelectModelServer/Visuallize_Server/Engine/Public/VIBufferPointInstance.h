#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferPointInstance final : public CVIBuffer
{
private:
	CVIBufferPointInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferPointInstance(const CVIBufferPointInstance& rhs);
	virtual ~CVIBufferPointInstance() = default;

public:
	virtual HRESULT InitializePrototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg);

public:
	void Update(_float fTimeDelta);

public:
	virtual HRESULT Render();

private:
	_uint					m_iNumInstance = 0;
	_uint					m_iInstanceStride = 0;
	ID3D11Buffer*			m_pVBInstance = nullptr;

	_float*					m_pInstanceSpeeds = nullptr;

public:
	static CVIBufferPointInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END