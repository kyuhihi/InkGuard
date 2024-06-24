#pragma once

#include "VIBuffer.h"

BEGIN(Engine)
class CTransform;

class ENGINE_DLL CVIBufferParticle final : public CVIBuffer
{
public:
	enum ALGORITHM {
		ALGORITHM_NORMAL,
		ALGORITHM_ROTATE,
		ALGORITHEM_CONVERAGE,
		ALGORITHEM_SPREAD,
		ALGORITHM_TARGET,
		ALGORITHM_ALPHAUP,
		ALGORITHM_ALPHADOWN,
		ALGORITHM_RAIN,
		ALGORITHM_WATERRAIN,
		ALGORITHM_POWGLOW,
		ALGORITHM_POWPARTICLE,
		ALGORITHM_BIGPOW,
		ALGORITHM_END
	};

	enum TEXTURE_TYPE {
		TYPE_NORMAL,
		TYPE_PACK,
		TYPE_END
	};

	using PARTICLESYSTEM_DESC = struct tagParticleSystem {
		CTransform* pTransform = nullptr;
		_bool bLoop;
		_uint iMaxParticle;
		_float fActivePerSecond;
		wchar_t* pTextureName = nullptr;


		/* limit Time */
		_float fLimit;

		/* speed */
		_float fMinSpeed;
		_float fMaxSpeed;

		/* start position */
		_bool bIsPosZero;

		_float3 vMinStartPosition;
		_float3 vMaxStartPosition;

		/* size */
		_bool bIsBigger;
		_bool bIsSmaller;
		_bool bIsRandom;

		_float fSizeMin;
		_float fSizeMax;

		/* direction */
		_bool bIsNoDir;
		_bool bIsObjLook;
		_bool bIsObjOppsiteLook;
		_bool bIsDirZero;

		_float3 vMinDir;
		_float3 vMaxDir;

		/* color */
		_bool bIsRandColor;
		_bool bIsMaskColor;

		_float4 vStartColor;
		_float4 vEndColor;

		_float4 vMaskColor;

		/* other */
		_float3 vAngle;
		_float fWavelength;
		ALGORITHM eState;

		/* ALGORITHM_TARGET */
		_float3 vTargetPosition;

		/* animation */
		_bool bTextureLoop;
		TEXTURE_TYPE textureType;
		_float2 textureSize;
		_float2 packStart;
		_float2 packSize;

		_int iMaxFrameX;
		_float fNextFrameTime;
		_int iLoopStartFrame;

		tagParticleSystem()
			: fActivePerSecond(0.1f)
			, bIsPosZero(true)
			, vMinStartPosition{ 0.f, 0.f, 0.f }
			, vMaxStartPosition{ 0.f, 0.f, 0.f }
			, fMinSpeed(1.f)
			, fMaxSpeed(10.f)
			, bIsBigger(false)
			, bIsSmaller(false)
			, bIsRandom(true)
			, fSizeMin(20)
			, fSizeMax(20)
			, fWavelength(1.f)
			, eState(ALGORITHM_NORMAL)
			, bIsNoDir(true)
			, bIsObjLook(false)
			, bIsObjOppsiteLook(false)
			, bIsDirZero(false)

			, textureType(TYPE_NORMAL)
			, textureSize(256.f, 256.f)
			, packStart(0.f, 0.f)
			, packSize(256.f, 256.f)

			, bTextureLoop(true)
			, fNextFrameTime(0.01f)
			, iLoopStartFrame(0)
			, iMaxFrameX(2)
			, bIsMaskColor(false)
			, bIsRandColor(true)
			, vMaskColor(1.f, 1.f, 1.f, 1.f)
			, vStartColor(1.f, 1.f, 1.f, 1.f)
			, vEndColor(1.f, 1.f, 1.f, 1.f)

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

		_float3 vStartPosition;

		_float fLifeTime;

		_float fCurrentPlayTime;
		_int iCurrentAnimeFrame;
		_int iAnimeMotion;
		_float2 vDrawPoint;
		_matrix mMatrix;
		_int iRandLimit;

		tagParticle()
			: bActive(true)
			, vAngle{ 0.f, 0.f, 0.f }
			, vColor(_float4(1.f, 1.f, 1.f, 1.f))
			, vDrawPoint(0.f, 0.f)
			, fCurrentPlayTime(0.f)
			, iCurrentAnimeFrame(0)
			, iAnimeMotion(0)
			, mMatrix(XMMatrixIdentity())
			, iRandLimit(0)
		{

		}

	};

private:
	CVIBufferParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferParticle(const CVIBufferParticle& rhs);
	virtual ~CVIBufferParticle() = default;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT RunCreateParticle(PARTICLESYSTEM_DESC desc);

public:
	void Update(_float fTimeDelta, CTransform* pTransform);
	_bool GetIsPlaying() const { return !(m_iActiveCount <= 1); }
	_bool GetbIsFinishParticle() { return m_bIsFinished; }


private:
	void UpdateAnimationTexture(VTXPARTICLEINSTANCE& pInstance, PARTICLE_DESC& particleInfo, _float fTimeDelta);
	_vector GetNextPosition(_vector vCurrentPosition, PARTICLE_DESC& particleInfo, _float fTimeDelta);

	_float SmoothStopArch3(_float t)
	{
		return t * (powf(1.f - t, 2.f));
	}


public:
	virtual HRESULT Render(ID3D11DeviceContext* pContext) override;
	 
private:
	_uint m_iNumInstance;
	_uint m_iNumPrimitive;
	_uint m_iInstanceStride;
	ID3D11Buffer* m_pVBInstance;

	_float* m_pInstanceSpeeds;

	PARTICLESYSTEM_DESC m_tSystemDesc;
	PARTICLE_DESC* m_tParticleInfos;
	_int m_iActiveCount = 0;
	_float3 m_fRight;
	_float3 m_fDir;

	_float m_fCurrentActiveTime;
	_bool m_bIsFinished = false;
	_uint m_iActiveLength; // 점점 하나씩 활성화시켜 초마다 생성된것처럼 보이게함


public:
	static CVIBufferParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END