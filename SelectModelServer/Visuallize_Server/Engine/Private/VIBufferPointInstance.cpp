#include "..\Public\VIBufferPointInstance.h"

CVIBufferPointInstance::CVIBufferPointInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBufferPointInstance::CVIBufferPointInstance(const CVIBufferPointInstance & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)	
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_pInstanceSpeeds(rhs.m_pInstanceSpeeds)
{
	SafeAddRef(m_pVBInstance);
}

HRESULT CVIBufferPointInstance::InitializePrototype(_uint iNumInstance)
{
#pragma region VERTEXBUFFER
	m_iNumInstance = iNumInstance;
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iStride = sizeof(VTXPOINT);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOINT*		pVertices = new VTXPOINT;

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices->vSize = _float2(0.5f, 0.5f);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	SafeDeleteArray(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = iNumInstance;
	m_iIndexSizeofPrimitive = sizeof(_ushort);
	m_iNumIndicesofPrimitive = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort*		pIndices = new _ushort[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumPrimitives);	

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	SafeDeleteArray(pIndices);

#pragma endregion


#pragma region INSTANCEBUFFER
	m_iInstanceStride = sizeof(VTXINSTANCE);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iInstanceStride * iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	VTXINSTANCE*		pInstanceVtx = new VTXINSTANCE[iNumInstance];
	ZeroMemory(pInstanceVtx, sizeof(VTXINSTANCE) * iNumInstance);

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		pInstanceVtx[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
		pInstanceVtx[i].vUp = _float4(0.f, 1.0f, 0.f, 0.f);
		pInstanceVtx[i].vLook = _float4(0.f, 0.f, 1.0f, 0.f);
		pInstanceVtx[i].vPosition = _float4(static_cast<_float>(rand() % 3 - 1.5f), 0.f, static_cast<_float>(rand() % 3 - 1.5f), 1.f);
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pInstanceVtx;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	SafeDeleteArray(pInstanceVtx);

	m_pInstanceSpeeds = new _float[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
		m_pInstanceSpeeds[i] = static_cast<_float>(rand() % 5 + 5);

#pragma endregion


	return S_OK;
}

HRESULT CVIBufferPointInstance::Initialize(void * pArg)
{
	return S_OK;
}

void CVIBufferPointInstance::Update(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXINSTANCE*)MappedSubResource.pData)[i].vPosition.y += m_pInstanceSpeeds[i] * fTimeDelta;
		if (3.0f <= ((VTXINSTANCE*)MappedSubResource.pData)[i].vPosition.y)
			((VTXINSTANCE*)MappedSubResource.pData)[i].vPosition.y = 0.f;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBufferPointInstance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance, 
	};

	_uint			iStrides[] = {
		m_iStride, 
		m_iInstanceStride
	};

	_uint			iOffsets[] = {
		0,
		0, 
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexedInstanced(1, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

CVIBufferPointInstance * CVIBufferPointInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance)
{
	CVIBufferPointInstance*			pInstance = new CVIBufferPointInstance(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(iNumInstance)))
	{
		MSG_BOX(TEXT("Failed To Created : CVIBufferRectInstance"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent * CVIBufferPointInstance::Clone(void * pArg)
{
	CVIBufferPointInstance*			pInstance = new CVIBufferPointInstance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVIBufferRectInstance"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CVIBufferPointInstance::Free()
{
	__super::Free();

	if(false == m_isCloned)
		SafeDeleteArray(m_pInstanceSpeeds);

	SafeRelease(m_pVBInstance);
}
