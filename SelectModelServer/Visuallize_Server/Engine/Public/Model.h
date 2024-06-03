#pragma once

#include "Component.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	class CHierarchyNode* GetHierarchyNode(const char* pNodeName);

	_uint GetNumMeshes() const {
		return m_iNumMeshes;
	}

	_uint GetMaterialIndex(_uint iMeshIndex);

	void SetAnimIndex(_uint iAnimIndex) {
		m_iCurrentAnimIndex = iAnimIndex;
	}

	_matrix GetPivotMatrix() {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

	//변경할 애니메이션 인덱스, 보간속도, 애니메이션이 끝난후에 키프레임을 처음으로 돌릴건지 true 돌림 false 안돌림
	void ChangeAnimation(_uint iNextAnimIndex, _float fBlendTime = 0.25f, _bool bLoop = true);
	_bool IsAnimEnd()
	{
		return m_bAnimEnd;
	}
	
	/* For. AnimationTool */
	_uint GetNumAnims();
	_uint GetNumKeyFrame(_uint iAnimIndex);
	vector<const char*>* GetAnimations()
	{
		return &m_AnimationsName;
	}

	void StopAnimation()
	{
		m_bPlay = false;
	}
	void ResumeAnimation()
	{
		m_bPlay = true;
	}
	void ShowHierarchy();

	typedef void(*FnPtr)(void);
	
	//애니메이션 인덱스, 몇번째 키프레임, 함수 포인터 맵, 함수 이름
	void SetKeyFrameEvent(_uint iAnimIndex, _uint iKeyFrame, const char* EventName);
	
	//현재 애니메이션의 몇번째 키프레임인지 표현을 위한 함수
	_uint GetCurrentKeyFrame(_uint iAnimIndex);
public:
	virtual HRESULT InitializePrototype(TYPE eType, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix, _bool bPicking);
	virtual HRESULT Initialize(void* pArg);


public:
	HRESULT SetUpOnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName);
	/* 애니메이션을 재생한다. */
	/* 1. 해당 애니메이션에서 사용하는 모든 뼈들의  Transformation 행렬을 갱신한다. */
	/* 2. Transformation를 최상위 부모로부터 자식으로 계속 누적시켜간다.(CombinedTransformation) */
	/* 3. 애니메이션에 의해 움직인 뼈들의 CombinedTransfromation을 셋팅한다. */
	HRESULT PlayAnimation(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iTechnique = 0, _uint iPassIndex = 0);
	HRESULT PlayAnimation(_float fTimeDelta, vector<string>* EventName);
	_bool	Picking(class CTransform* pTransform, _float3 * pOut);

private:
	const aiScene*				m_pAIScene = nullptr;
	Assimp::Importer			m_Importer;

	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = TYPE_END;

private:
	_uint m_iNumMeshes = 0;
	using MESHES = vector<class CMeshContainer*>;
	MESHES m_Meshes;

private:
	_uint m_iNumMaterials = 0;
	vector<MATERIALDESC> m_Materials;

private:
	vector<class CHierarchyNode*> m_HierarchyNodes;

private:
	_uint m_iCurrentAnimIndex = 0;
	_uint m_iNumAnimations = 0;
	vector<class CAnimation*> m_Animations;
	
	/* For. AnimationTool */
	/* 항상 Animations의 개수와 AnimationsName의 개수는 같다 */
	vector<const char*>					m_AnimationsName;
	_bool								m_bPlay = true;


private:
	HRESULT ReadyMeshContainers(_fmatrix PivotMatrix, _bool bPicking);
	HRESULT ReadyMaterials(const char* pModelFilePath);
	HRESULT ReadyHierarchyNodes(aiNode* pNode, class CHierarchyNode* pParent, _uint iDepth);
	HRESULT ReadyAnimations();

private:
	_bool m_bAnimEnd = false;

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix = XMMatrixIdentity(), _bool bPicking = false);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END