#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferTerrain final : public CVIBuffer
{
private:
	CVIBufferTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferTerrain(const CVIBufferTerrain& rhs);
	virtual ~CVIBufferTerrain() = default;

public:
	virtual HRESULT InitializePrototype(const _tchar* pHeighitMapFilePath);
	virtual HRESULT Initialize(void* pArg);

public:
	void Culling(const class CTransform* pTransform);

private:
	_uint			m_iNumVerticesX = 0;
	_uint			m_iNumVerticesZ = 0;
	FACEINDICES32*	m_pFaceIndices = nullptr;
	class CQuadTree*		m_pQuadTree = nullptr;

public:
	static CVIBufferTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeighitMapFilePath);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END