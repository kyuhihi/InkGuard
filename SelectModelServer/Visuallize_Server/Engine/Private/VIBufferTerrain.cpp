#include "..\Public\VIBufferTerrain.h"
#include "Frustum.h"
#include "Transform.h"
#include "QuadTree.h"

CVIBufferTerrain::CVIBufferTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBufferTerrain::CVIBufferTerrain(const CVIBufferTerrain & rhs)
	: CVIBuffer(rhs)
	, m_pFaceIndices(rhs.m_pFaceIndices)	
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_pQuadTree(rhs.m_pQuadTree)
{
	SafeAddRef(m_pQuadTree);
}

HRESULT CVIBufferTerrain::InitializePrototype(const _tchar* pHeighitMapFilePath)
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(pHeighitMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER			fh;
	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);

	BITMAPINFOHEADER			ih;
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	_ulong*						pPixel = new _ulong[ih.biWidth * ih.biHeight];
	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);

#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iStride = sizeof(VTXNORTEX);
	
	m_pVerticesPos = new _float3[m_iNumVertices];
	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(static_cast<_float>(j), (pPixel[iIndex] & 0x000000ff) / 10.0f, static_cast<_float>(i));
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexture = _float2(j / _float(m_iNumVerticesX - 1), i / _float(m_iNumVerticesZ - 1));
		}
	}
	


	SafeDeleteArray(pPixel);

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	m_pFaceIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(m_pFaceIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSourDir, vDestDir, vNormal;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];

			vSourDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal, 
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);
			
			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];

			vSourDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);
			
			++iNumFaces;
		}
	}	

	for (_uint i = 0; i < m_iNumVertices; ++i)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	SafeDeleteArray(pVertices);
	SafeDeleteArray(pIndices);

#pragma endregion

	_uint		iLT = m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX;
	_uint		iRT = m_iNumVerticesX * m_iNumVerticesZ - 1;
	_uint		iRB = m_iNumVerticesX - 1;
	_uint		iLB = 0;

	m_pQuadTree = CQuadTree::Create(iLT, iRT, iRB, iLB);
	if (nullptr == m_pQuadTree)
		return E_FAIL;

	if (FAILED(m_pQuadTree->MakeNeighbor()))
		return E_FAIL;


	return S_OK;
}

HRESULT CVIBufferTerrain::Initialize(void * pArg)
{
	return S_OK;
}

void CVIBufferTerrain::Culling(const CTransform* pTransform)
{
	CFrustum*			pFrustum = GET_INSTANCE(CFrustum);

	pFrustum->TransformToLocalSpace(pTransform->GetWorldMatrixInverse());	

	/*ZeroMemory(m_pFaceIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);*/

	_uint			iNumFaces = 0;

	//for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	//{
	//	for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
	//	{
	//		_uint		iIndex = i * m_iNumVerticesX + j;

	//		_uint		iIndices[] = {
	//			iIndex + m_iNumVerticesX,
	//			iIndex + m_iNumVerticesX + 1,
	//			iIndex + 1,
	//			iIndex
	//		};

	//		_bool		isIn[] = {
	//			pFrustum->isInLocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[0]])),
	//			pFrustum->isInLocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[1]])),
	//			pFrustum->isInLocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[2]])),
	//			pFrustum->isInLocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[3]])),
	//		};

	//		/* 오른쪽 위 삼각형을 그려야한다. */
	//		if (true == isIn[0] &&
	//			true == isIn[1] &&
	//			true == isIn[2])
	//		{
	//			m_pFaceIndices[iNumFaces]._0 = iIndices[0];
	//			m_pFaceIndices[iNumFaces]._1 = iIndices[1];
	//			m_pFaceIndices[iNumFaces]._2 = iIndices[2];
	//			++iNumFaces;
	//		}

	//		/* 왼쪽 하단 삼각형을 그려야한다. */
	//		if (true == isIn[0] &&
	//			true == isIn[2] &&
	//			true == isIn[3])
	//		{
	//			m_pFaceIndices[iNumFaces]._0 = iIndices[0];
	//			m_pFaceIndices[iNumFaces]._1 = iIndices[2];
	//			m_pFaceIndices[iNumFaces]._2 = iIndices[3];
	//			++iNumFaces;
	//		}
	//	}
	//}

	m_pQuadTree->Culling(m_pVerticesPos, pFrustum, m_pFaceIndices, &iNumFaces);

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, m_pFaceIndices, sizeof(FACEINDICES32) * iNumFaces);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumPrimitives = iNumFaces;

	RELEASE_INSTANCE(CFrustum);
}

CVIBufferTerrain * CVIBufferTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pHeighitMapFilePath)
{
	CVIBufferTerrain*			pInstance = new CVIBufferTerrain(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(pHeighitMapFilePath)))
	{
		MSG_BOX(TEXT("Failed To Created : CVIBufferTerrain"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent * CVIBufferTerrain::Clone(void * pArg)
{
	CVIBufferTerrain*			pInstance = new CVIBufferTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVIBufferTerrain"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CVIBufferTerrain::Free()
{
	__super::Free();

	SafeRelease(m_pQuadTree);

	if (false == m_isCloned)
		SafeDeleteArray(m_pFaceIndices);
}
