#pragma once


#include "VIBuffer.h"
#include "Model.h"


/* 모델을 구성하는 하나의 메시. */
/* 이 메시를 구성하는 정점, 인덱스 버퍼를 보관한다. */

BEGIN(Engine)

class CMeshContainerInstance final : public CVIBuffer
{
public:
	enum SPEEDOPTION
	{
		SPEEDNORMAL,
		EASEIN,
		EASEOUT
	};

	enum ALGORITHM {
		ALGORITHM_NORMAL,
		ALGORITHM_ROTATE,
		ALGORITHEM_CONVERAGE,
		ALGORITHEM_SPREAD,
		ALGORITHM_DOUNUT,
		ALGORITHM_SPARK,
		ALGORITHM_DOOR,
		ALGORITHM_KINOPIODOOR,
		ALGORITHM_CHIKUWABLOCK_SIDE,
		ALGORITHM_CHIKUWABLOCK_BACKFOWARD,
		ALG0RITHM_KYUPPONFIRE,
		ALGORITHM_LEAFEFFECT,
		ALGORITHM_COINRAINEFFECT,
		ALGORITHM_STARRAIN,
		ALGORITHM_KOOPABLOCK_SIDE,
		ALGORITHM_KOOPABLOCK_BACKFOWARD,
		ALGORITHM_WALLSMOKE,
		ALGORITHM_WALLPARTICLE,
		ALGORITHM_BIGMUSHROOM,
		ALGORITHM_STAIRSMOKE,
		ALGORITHM_STAIR,
		ALGORITHM_KOOPALAVA,
		ALGORITHM_KOOPALAVABUBBLE,
		ALGORITHM_BIGPOWEFFECT,
		ALGORITHM_STEPLONG,
		ALGORITHM_STEPLONGSMOKE,
		ALGORITHM_KOOPAFIRE,
		ALGORITHM_WOODBOX,
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


		_float3 vAngle;
		_float fWavelength;
		ALGORITHM eState;

		/* ALGORITHM_TARGET */
		_float3 vTargetPosition;

		SPEEDOPTION iSpeedOption;


		/* Rotation */
		_bool bIsRotation;
		_float3 vRoataionAxis;
		_float fRotationAngle;

		/* color */
		_bool bIsMaskColor;
		_bool bIsTextureColor;

		_float4 vMaskColor;

		_int iColorTextureNum;
		_int iTextureXNum;
		_int iTextureYNum;


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
			, iSpeedOption(SPEEDNORMAL)
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

	using PARTICLE_DESC = struct tagParticle {
		_bool bActive;
		_float3 vDirection;
		_float fSpeed;
		_float4 vColor;
		_float3 vAngle;
		_float vSize;
		_float fLifeTime;
		_float3 vStartPosition;
		_float vCurrSize;
		_bool bSizeMax;
		_bool bSizeMin;
		_float fDuration;
		_matrix mMatrix;
		_bool bIsRender;
		_bool bIsNonTick;

		tagParticle()
			: bActive(true)
			, vAngle(_float3(0.f, 0.f, 0.f))
			, vColor(_float4(1.f, 1.f, 1.f, 1.f))
			, bSizeMax(false)
			, bSizeMin(false)
			, fDuration(0.f)
			, mMatrix(XMMatrixIdentity())
			, bIsRender(true)
			, bIsNonTick(false)
		{

		}

	};

private:
	CMeshContainerInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainerInstance(const CMeshContainerInstance& rhs);
	virtual ~CMeshContainerInstance() = default;

public:
	_uint GetMaterialIndex() const {
		return m_iMaterialIndex;
	}

	_bool GetbIsEffectFinish() {
		return m_bIsEffectFinish;
	}

public:
	virtual HRESULT InitializePrototype(PARTICLESYSTEM_DESC desc, aiMesh* pAIMesh, class CInstanceModel* pModel, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	void Update(_float fTimeDelta, CTransform* pTransform);
	_vector GetNextPosition(_vector vCurrentPosition, PARTICLE_DESC & particleInfo, _float fTimeDelta);
	HRESULT SetUpHierarchyNodes(class CInstanceModel* pModel, aiMesh* pMesh);
	void SetUpBoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

	virtual HRESULT Render(ID3D11DeviceContext* pContext) override;

private:
	_float EaseIn(_float t);
	_float EaseOut(_float t);
	_float Flip(_float x);
	_float Lerp(_float fA, _float fB, _float fAlpha);
	_float SmoothStopArch3(_float t);
private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = 0;
private:
	/* 이 메시에 영향ㅇ르 주는 뼈의 갯수. */
	_uint							m_iNumBones = 0;

	/* 이 메시에 영향을 주는 뼈들을 모아놓느다. */
	/* why ? : 메시 별로 렌더링할 때 이 메시에 영향ㅇ르 주는 뼏르의 행렬을 모아서 셰{ㅇ디러ㅗ 도ㅓㄴ질려고. */
	vector<class CHierarchyNode*>	m_Bones;

	ID3D11Buffer*			m_pVBInstance = nullptr;
	_uint					m_iInstanceStride;
	_uint					m_iNumInstance;
	_uint					m_iNumIndices;

private:
	PARTICLESYSTEM_DESC		m_tSystemDesc;
	PARTICLE_DESC*			m_tParticleInfos;
	_uint					m_iActiveCount;
	_float m_fCurrentActiveTime;
	_uint m_iActiveLength; // 점점 하나씩 활성화시켜 초마다 생성된것처럼 보이게함

	_bool					m_bIsEffectFinish = false;


private:
	_float3 m_fDir;
	_float3 m_fRight;
	_float m_fSpeed = 0.f;
	_float4 m_fPosition = { 0.f, 0.f, 0.f, 1.f };
	_float m_fSize = 0.f;
	_matrix	m_mDount;
	_float m_fAngle = 0.f;
	_float4 m_vDisPos[240];
	_float fAddY[240] = { 0.f };
	_float4 m_fSavePos[240];
	_float m_fTimeDelta = 0.f;
	_uint m_iCount = 1;
	_float miniStarSize = 0.35f;

private:
	_float					m_fDuration = 0.f;

private:
	HRESULT ReadyVertices(aiMesh* pMesh, _fmatrix PivotMatrix);

public:
	static CMeshContainerInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PARTICLESYSTEM_DESC desc, aiMesh* pMesh, class CInstanceModel* pModel, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END