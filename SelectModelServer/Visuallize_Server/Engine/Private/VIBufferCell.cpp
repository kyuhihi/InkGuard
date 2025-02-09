#include "..\Public\VIBufferCell.h"

CVIBufferCell::CVIBufferCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBufferCell::CVIBufferCell(const CVIBufferCell & rhs)
	: CVIBuffer(rhs)
	
{

}

HRESULT CVIBufferCell::InitializePrototype(const _float3* pPoints)
{
#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 3;
	m_iStride = sizeof(VTXCOL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXCOL*		pVertices = new VTXCOL[4];
	ZeroMemory(pVertices, sizeof(VTXCOL) * 4);

	pVertices[0].vPosition = pPoints[0];
	pVertices[0].vColor = _float4(0.f, 1.f, 0.f, 1.f);

	pVertices[1].vPosition = pPoints[1];
	pVertices[1].vColor = _float4(0.f, 1.f, 0.f, 1.f);

	pVertices[2].vPosition = pPoints[2];
	pVertices[2].vColor = _float4(0.f, 1.f, 0.f, 1.f);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	SafeDeleteArray(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
	/*m_iNumPrimitives = 4;*/
	m_iIndexSizeofPrimitive = sizeof(_ushort);
	/*m_iNumIndicesofPrimitive = 2;*/
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = 4 * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	_ushort*		pIndices = new _ushort[4];
	ZeroMemory(pIndices, sizeof(_ushort) * 4);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;	

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	SafeDeleteArray(pIndices);

#pragma endregion


	return S_OK;
}

HRESULT CVIBufferCell::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBufferCell::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
	};

	_uint			iStrides[] = {
		m_iStride
	};

	_uint			iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexed(4, 0, 0);

	return S_OK;
}

CVIBufferCell * CVIBufferCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3* pPoints)
{
	CVIBufferCell*			pInstance = new CVIBufferCell(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(pPoints)))
	{
		MSG_BOX(TEXT("Failed To Created : CVIBufferCell"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent * CVIBufferCell::Clone(void * pArg)
{
	CVIBufferCell*			pInstance = new CVIBufferCell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVIBufferCell"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CVIBufferCell::Free()
{
	__super::Free();
}
