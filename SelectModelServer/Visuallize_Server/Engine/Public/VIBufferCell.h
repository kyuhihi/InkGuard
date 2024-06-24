#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferCell final : public CVIBuffer
{
private:
	CVIBufferCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferCell(const CVIBufferCell& rhs);
	virtual ~CVIBufferCell() = default;

public:
	virtual HRESULT InitializePrototype(const _float3* pPoints);
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Render();

public:
	static CVIBufferCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END