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

	//������ �ִϸ��̼� �ε���, �����ӵ�, �ִϸ��̼��� �����Ŀ� Ű�������� ó������ �������� true ���� false �ȵ���
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
	
	//�ִϸ��̼� �ε���, ���° Ű������, �Լ� ������ ��, �Լ� �̸�
	void SetKeyFrameEvent(_uint iAnimIndex, _uint iKeyFrame, const char* EventName);
	
	//���� �ִϸ��̼��� ���° Ű���������� ǥ���� ���� �Լ�
	_uint GetCurrentKeyFrame(_uint iAnimIndex);
public:
	virtual HRESULT InitializePrototype(TYPE eType, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix, _bool bPicking);
	virtual HRESULT Initialize(void* pArg);


public:
	HRESULT SetUpOnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName);
	/* �ִϸ��̼��� ����Ѵ�. */
	/* 1. �ش� �ִϸ��̼ǿ��� ����ϴ� ��� ������  Transformation ����� �����Ѵ�. */
	/* 2. Transformation�� �ֻ��� �θ�κ��� �ڽ����� ��� �������Ѱ���.(CombinedTransformation) */
	/* 3. �ִϸ��̼ǿ� ���� ������ ������ CombinedTransfromation�� �����Ѵ�. */
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
	/* �׻� Animations�� ������ AnimationsName�� ������ ���� */
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