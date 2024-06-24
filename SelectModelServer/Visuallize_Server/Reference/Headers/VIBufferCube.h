#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferCube final : public CVIBuffer
{
private:
	CVIBufferCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferCube(const CVIBufferCube& rhs);
	virtual ~CVIBufferCube() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CVIBufferCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END