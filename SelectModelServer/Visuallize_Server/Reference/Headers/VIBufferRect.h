#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferRect final : public CVIBuffer
{
private:
	CVIBufferRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferRect(const CVIBufferRect& rhs);
	virtual ~CVIBufferRect() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CVIBufferRect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END