#pragma once


#include "VIBuffer.h"
#include "Model.h"


/* 모델을 구성하는 하나의 메시. */
/* 이 메시를 구성하는 정점, 인덱스 버퍼를 보관한다. */

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint GetMaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT InitializePrototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix, _bool bPiking);
	virtual HRESULT Initialize(void* pArg);	

public:
	HRESULT SetUpHierarchyNodes(class CModel* pModel, aiMesh* pAIMesh);
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);


private:
	char m_szName[MAX_PATH] = "";
	_uint m_iMaterialIndex = 0;
private:
	_uint m_iNumBones = 0;

	vector<class CHierarchyNode*> m_Bones;

private:
	HRESULT ReadyVertices(const aiMesh* pAIMesh, _fmatrix PivotMatrix, _bool bPicking);
	HRESULT Ready_AnimVertices(const aiMesh* pAIMesh, CModel* pModel);

public:
	virtual _bool Picking(class CTransform* pTransform, _float3* pOut, aiMesh* pAIMesh);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix,_bool bPicking);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END