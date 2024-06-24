#pragma once

#include "Component.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CInstanceModel final : public CComponent
{
public:
	enum ALGORITHM {
	ALGORITHM_NORMAL,
	ALGORITHM_ROTATE,
	ALGORITHEM_CONVERAGE,
	ALGORITHEM_SPREAD,
	ALGORITHM_DOUNUT,
	ALGORITHM_END
};

using PARTICLESYSTEM_DESC = struct tagParticleSystem {
	
	CTransform* pTransform = nullptr;
	
	_bool bLoop;
	_uint iMaxParticle;
	_float fActivePerSecond;


	/* limit Time */
	_float fLimit;

	/* speed */
	_float fMinSpeed;
	_float fMaxSpeed;

	/* start position */
	_bool bIsZeroPos;

	_float3 vMinStartPosition;
	_float3 vMaxStartPosition;

	/* size */
	_bool bIsBigger;
	_bool bIsSmaller;
	_bool bIsBiggerSmaller;
	_bool bIsRandom;


	_float fSizeMin;
	_float fSizeMax;

	/* direction */
	_bool bIsNoDir;
	_bool bIsObjLook;
	_bool bIsObjOppsiteLook;

	_float3 vMinDir;
	_float3 vMaxDir;

	/* color */
	_bool bIsMaskColor;
	_bool bIsTextureColor;

	_float4 vMaskColor;

	_int iColorTextureNum;
	_int iTextureXNum;
	_int iTextureYNum;

	_float3 vAngle;
	_float fWavelength;
	ALGORITHM eState;

	/* ALGORITHM_TARGET */
	_float3 vTargetPosition;

	_int iSpeedOption;

	/* Rotation */
	_bool bIsRotation;
	_float3 vRoataionAxis;
	_float fRotationAngle;


	tagParticleSystem()
		: fActivePerSecond(0.1f)
		, bIsZeroPos(false)
		, vMinStartPosition{ 0.f, 0.f, 0.f }
		, vMaxStartPosition{ 0.f, 0.f, 0.f }
		, fMinSpeed(1.f)
		, fMaxSpeed(10.f)
		, bIsBigger(false)
		, bIsSmaller(false)
		, bIsBiggerSmaller(false)
		, bIsRandom(true)
		, fSizeMin(20)
		, fSizeMax(20)
		, bIsNoDir(false)
		, bIsObjLook(false)
		, bIsObjOppsiteLook(false)
		, eState(ALGORITHM_NORMAL)
		, iSpeedOption(0)
		, bIsRotation(false)
		, vRoataionAxis(0.f, 0.f, 0.f)
		, fRotationAngle(0.f)
		, bIsTextureColor(false)
		, bIsMaskColor(false)
		, vMaskColor(_float4(1.f, 1.f, 1.f, 1.f))
		, iColorTextureNum(0)
		, iTextureXNum(0)
		, iTextureYNum(0)

	{

	}
};
public:
	enum TYPE { TYPE_NONANIM, TYPE_END };
private:
	CInstanceModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstanceModel(const CInstanceModel& rhs);
	virtual ~CInstanceModel() = default;

public:
	class CHierarchyNode* GetHierarchyNode(const char* pNodeName);

	_uint GetNumMeshes() const {
		return m_iNumMeshes;
	}

	_uint GetMaterialIndex(_uint iMeshIndex);

	_matrix GetPivotMatrix() {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

	_bool GetIsFinished() { return m_bIsFinished; }
	
public:
	virtual HRESULT InitializePrototype(TYPE eType, const char* pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	void Update(_float fTimeDelta, CTransform* pTransform);



public:
	HRESULT SetUpOnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName);
	/* 애니메이션을 재생한다. */
	/* 1. 해당 애니메이션에서 사용하는 모든 뼈들의  Transformation 행렬을 갱신한다. */
	/* 2. Transformation를 최상위 부모로부터 자식으로 계속 누적시켜간다.(CombinedTransformation) */
	/* 3. 애니메이션에 의해 움직인 뼈들의 CombinedTransfromation을 셋팅한다. */
	HRESULT Render(ID3D11DeviceContext* pContext, class CShader* pShader, _uint iMeshIndex, _uint iTechnique = 0, _uint iPassIndex = 0);

private:
	const aiScene*				m_pAIScene = nullptr;
	Assimp::Importer			m_Importer;

	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = TYPE_END;

private:
	_uint m_iNumMeshes = 0;
	using MESHES = vector<class CMeshContainerInstance*>;
	MESHES m_Meshes;

private:
	_uint m_iNumMaterials = 0;
	vector<MATERIALDESC> m_Materials;
	_bool m_bIsFinished = false;

private:
	char m_szFilePath[MAX_PATH] = "";
	PARTICLESYSTEM_DESC m_eDesc;
	vector<class CHierarchyNode*> m_HierarchyNodes;

private:
	HRESULT ReadyMeshContainers(_fmatrix PivotMatrix);
	HRESULT ReadyMaterials(const char* pModelFilePath);
	HRESULT ReadyHierarchyNodes(aiNode* pNode, class CHierarchyNode* pParent, _uint iDepth);

public:
	static CInstanceModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END