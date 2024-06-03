#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CComputeShader final : public CComponent
{
private:
	CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComputeShader(const CComputeShader& rhs);
	virtual ~CComputeShader() = default;

public:
	virtual HRESULT InitializePrototype(const _tchar* pShaderFilePath, const char* pEntryPointName);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT SetOutput(ID3D11UnorderedAccessView* const* ppUAVArray, _uint NumUAVArray, _uint StartArraySlot = 0, const _uint* pUAVInitialCounts = NULL);
	HRESULT SetInput(ID3D11ShaderResourceView* const* ppSRVArray, _uint NumUAVArray, _uint StartArraySlot = 0);
	HRESULT SetShader(ID3D11ClassInstance* const* ppClassInstances = NULL, _uint NumClassInstances = 0, _uint iReset = 0);
	HRESULT Dispatch(_uint ThreadGroupCountX = 1, _uint ThreadGroupCountY = 1, _uint ThreadGroupCountZ = 1);

private:
	ID3D11ComputeShader* m_pComputeShader;
	//ID3D11Buffer* m_pBuffer;

public:
	static CComputeShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const char* pEntryPointName);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END