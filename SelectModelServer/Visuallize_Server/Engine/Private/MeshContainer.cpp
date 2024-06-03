#include "..\Public\MeshContainer.h"
#include "Model.h"
#include "HierarchyNode.h"
#include "Picking.h"

CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: CVIBuffer(rhs)	
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CMeshContainer::InitializePrototype(CModel::TYPE eModelType, const aiMesh * pAIMesh, CModel* pModel, _fmatrix PivotMatrix, _bool bPicking)
{
	/* �� �޽ÿ� �̸��� ���� ���밡 �����Ѵٸ�. 
	�� ������ ����� �޽ø� �����ϴ� ������ ������ �� �ֵ��� �����ϳ�. */
	strcpy_s(m_szName, pAIMesh->mName.data);

	/* �޽ø��� ����ϴ� ���׸���(�ؽ��������� ǥ��)�� �ٸ���. */
	/* �޽ø� �׸��� ���� � ���׸����� �ѵ�÷� �������� ������ �������ֱ����ؼ�. */
	m_iMaterialIndex = pAIMesh->mMaterialIndex;

#pragma region VERTEXBUFFER

	HRESULT		hr = 0;

	if (CModel::TYPE_NONANIM == eModelType)
		hr = ReadyVertices(pAIMesh, PivotMatrix, bPicking);
	else 
		hr = Ready_AnimVertices(pAIMesh, pModel);

	if (FAILED(hr))
		return E_FAIL;
	
#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = pAIMesh->mNumFaces;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; ++i)
	{
		pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
	}


	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	SafeDeleteArray(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMeshContainer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMeshContainer::SetUpHierarchyNodes(CModel * pModel, aiMesh* pAIMesh)
{
	m_iNumBones = pAIMesh->mNumBones;



	/* ���� �޽ÿ� ���⤷�� ��2�̴� ������ ��ȸ�Ѵ٤�. */
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i];

		CHierarchyNode*		pHierarchyNode = pModel->GetHierarchyNode(pAIBone->mName.data);

		_float4x4			OffsetMatrix;

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		pHierarchyNode->SetOffsetMatrix(XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_Bones.push_back(pHierarchyNode);

		SafeAddRef(pHierarchyNode);
	}

	if (0 == m_iNumBones)
	{

		CHierarchyNode*		pNode = pModel->GetHierarchyNode(m_szName);

		if (nullptr == pNode)
			return S_OK;

		m_iNumBones = 1;

		m_Bones.push_back(pNode);

	}

	return S_OK;
}

/* �޽��� ������ �׸������� ���̴��� �ѱ������ ������� �迭�� �����Ѵ�. */
void CMeshContainer::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
{
	if (0 == m_iNumBones)
	{
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());
		return;
	}

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat4x4(&pBoneMatrices[i], XMMatrixTranspose(m_Bones[i]->GetOffSetMatrix() * m_Bones[i]->GetCombinedTransformation() * PivotMatrix));
	}
	
}

HRESULT CMeshContainer::ReadyVertices(const aiMesh* pAIMesh, _fmatrix PivotMatrix, _bool bPicking)
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMODEL*		pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	if (bPicking)
	{
		m_pVerticesPos = new _float3[m_iNumVertices];
		ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);
	}

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		/* ������ ��ġ�� ���� ���ϴ� ���ʱ���·� ��ȭ������./ */
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		/* ������ ��ġ�� �ٲ���⶧������ ��ֵ� �ٲ��. */
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		memcpy(&pVertices[i].vTexture, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PivotMatrix));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	if (bPicking)
	{
		for (_uint j = 0; j < m_iNumVertices; ++j)
			m_pVerticesPos[j] = pVertices[j].vPosition;
	}

	SafeDeleteArray(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Ready_AnimVertices(const aiMesh* pAIMesh, CModel* pModel)
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iStride = sizeof(VTXANIMMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXANIMMODEL*		pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		/* ������ȯ( x) : ���� ��İ� �������� �׷�����. 
		������ȯ�� ���� ������ ������ ��Ƴ��� ���̴�. */
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexture, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));		
	}

	/* ���� �޽ÿ� ���⤷�� ��2�̴� ������ ��ȸ�Ѵ٤�. */
	/* ��(aiBone)�ȿ� ǥ���Ǿ��ִ�, �̻��� � �������� ������ �ִ���(mVertexId)�� �޾ƿͼ�. 
	�ش� �������� �̻��� ������ �޴´�(vBlendIndex), �󸶳�(vBlendWeight)�� ��Ƶд�. */

	for (_uint i = 0; i <  pAIMesh->mNumBones; ++i)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i];	

		/* i��° ���� � �����鿡�� ���⤷�� �ִ��� ��ȸ�Ѵ�. */
		for (_uint j = 0; j < pAIBone->mNumWeights; ++j)
		{
			_uint		iVertexIndex = pAIBone->mWeights[j].mVertexId;

			if(0.0f == pVertices[iVertexIndex].vBlendWeight.x)
			{
				pVertices[iVertexIndex].vBlendIndex.x = i;
				pVertices[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.y)
			{
				pVertices[iVertexIndex].vBlendIndex.y = i;
				pVertices[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.z)
			{
				pVertices[iVertexIndex].vBlendIndex.z = i;
				pVertices[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.w)
			{
				pVertices[iVertexIndex].vBlendIndex.w = i;
				pVertices[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}
	
	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	SafeDeleteArray(pVertices);	

	return S_OK;
}

_bool CMeshContainer::Picking(CTransform * pTransform, _float3 * pOut, aiMesh * pAIMesh)
{
	CPicking* pPicking = CPicking::GetInstance();

	SafeAddRef(pPicking);

	_matrix		WorldMatrixInv = pTransform->GetWorldMatrixInverse();

	_vector			vRayDir, vRayPos;

	vRayDir = pPicking->Compute_LocalRayInfo(false, pTransform);
	vRayPos = pPicking->Compute_LocalRayInfo(true, pTransform);

	vRayDir = XMVector3Normalize((vRayDir));
	for (_uint i = 0; i < pAIMesh->mNumFaces; ++i)
	{//���ؽ����� 3���� ������ ���Ѵ�.
		_uint		iIndices[] = {
			(3 * i) + 0,
			(3 * i) + 1,
			(3 * i) + 2
		};
		
		_float		fDist;
		_matrix	WorldMatrix = pTransform->GetWorldMatrix();
		if (true == TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), XMLoadFloat3(&m_pVerticesPos[iIndices[1]]), XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), fDist))
		{
			_float3 vPickPos;
			XMStoreFloat3(&vPickPos, vRayPos + vRayDir * fDist);
			XMStoreFloat3(pOut, XMVector3TransformCoord(XMLoadFloat3(&vPickPos), WorldMatrix));

			SafeRelease(pPicking);
			return true;
		}
	}


	SafeRelease(pPicking);
	return false;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eModelType, const aiMesh * pAIMesh, CModel* pModel, _fmatrix PivotMatrix,_bool bPicking)
{
	CMeshContainer*			pInstance = new CMeshContainer(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(eModelType, pAIMesh, pModel, PivotMatrix, bPicking)))
	{
		MSG_BOX(TEXT("Failed To Created : CMeshContainer"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent * CMeshContainer::Clone(void * pArg)
{
	CMeshContainer*			pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMeshContainer"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CMeshContainer::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_Bones)
		SafeRelease(pHierarchyNode);

	if (nullptr != m_pVerticesPos)
	{
		SafeDeleteArray(m_pVerticesPos);
	}
	
	m_Bones.clear();

}
