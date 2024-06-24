#pragma once

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBufferWireMesh final : public CVIBuffer
{
public:
	using MESHDESC = struct tagMeshDesc
	{
		vector<_float3> vertices;
		vector<FACEINDICES32> indices;
	};
private:
	CVIBufferWireMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferWireMesh(const CVIBufferWireMesh& rhs);
	virtual ~CVIBufferWireMesh() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CVIBufferWireMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END