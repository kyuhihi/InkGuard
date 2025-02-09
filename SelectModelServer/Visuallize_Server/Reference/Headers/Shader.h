#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
public:
	using PASSDESC = struct tagPassDesc
	{
		ID3DX11EffectPass* pPass = nullptr;
		ID3D11InputLayout* pInputLayout = nullptr;
	};
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT InitializePrototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg);

public:
	/* 셰이더 전역변수에 데이터를 던진다. */
	HRESULT SetRawValue(const char* pContantName, const void* pData, _uint iByteLength);
	HRESULT SetMatrixArray(const char * pConstantName, const _float4x4* pData, _uint iNumMatrices);
	HRESULT SetShaderResourceView(const char* pContantName, ID3D11ShaderResourceView* pSRV);
	HRESULT SetShaderResourceViewArray(const char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTexture);

public: /* 이 셰이더의 특정 패스로 그린다. */
	HRESULT Begin(_uint iTechniqueIndex, _uint iPassIndex);

private:
	ID3DX11Effect* m_pEffect = nullptr;
	vector<vector<PASSDESC>> m_techniques;

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END