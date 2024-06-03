#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBufferTrail final : public CVIBuffer
{
public:
	enum RENDER_PASS
	{
		PASS_DEFAULT,
		PASS_NONE_TEXTURE,
		PASS_MASK,
		PASS_MIRROR_MASK,
		PASS_END
	};

	enum SIZE_BASE
	{
		BASE_RIGHT,
		BASE_UP,
		BASE_LOOK,
		BASE_END
	};

	using GRADIENT_DESC = struct tagGradientDesc
	{
		_float4 vColor;
		_float fRatio; //(0 ~ 1)
	};

	using TRAIL_DESC = struct tagTrailDesc
	{
		_float fSize;
		_float fUpdateIntervalTime;
		_uint iTrailLength = 256; // 트레일 정밀도 설정(최대 256)
		_float2 vUVRepeat = _float2(1.f, 1.f); // 이미지를 반복할지 설정(마이너스로 설정하면 이미지 반전가능)
		_bool bUVSwap = false; // U와 V를 교체함
		SIZE_BASE eSizeBase = BASE_UP; // 사이즈 기준 설정
		vector<tagGradientDesc> gradientColor;

		_float4 vFirstColor = _float4(1.f, 1.f, 1.f, 1.f);
		_float4 vEndColor = _float4(1.f, 1.f, 1.f, 0.f);
	};

private:
	CVIBufferTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBufferTrail(const CVIBufferTrail& rhs);
	virtual ~CVIBufferTrail() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(ID3D11DeviceContext* pContext);

public:
	void Update(_float fTimeDelta, _float4x4 worldMatrix);
	HRESULT SetUpOnShader(class CShader* pShader);

public:
	void SetUVRepeat(_float2 vRepeat) { m_tTrailDesc.vUVRepeat = vRepeat; }
	void SetUVSwap(_bool bSwap) { m_tTrailDesc.bUVSwap = bSwap; }
	void SetSizeBase(SIZE_BASE eSizeBase) { m_tTrailDesc.eSizeBase = eSizeBase; }

	void StopTracking() { m_bTracking = false; };
	void StartTracking(_float4x4 worldMatrix);

private:
	GRADIENT_DESC PickGradientStart(_float fRatio);
	GRADIENT_DESC PickGradientEnd(_float fRatio);

private:
	static const _uint TRAIL_MAX_LENGTH = 256;

private:
	TRAIL_DESC m_tTrailDesc;

	_uint m_iNumInstance;
	_uint m_iNumPrimitive;
	_uint m_iInstanceStride;
	ID3D11Buffer* m_pVBInstance;

	_float m_fCurrentTime;

	_float4 m_vPositions[TRAIL_MAX_LENGTH];
	_float4 m_vSizeBase[TRAIL_MAX_LENGTH];

	_bool m_bTracking;

public:
	static CVIBufferTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};
END