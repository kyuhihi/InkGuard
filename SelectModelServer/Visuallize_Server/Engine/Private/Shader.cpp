#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_techniques(rhs.m_techniques)
{
	for (auto& passes : m_techniques) {
		for (auto& passDesc : passes) {
			SafeAddRef(passDesc.pInputLayout);
			SafeAddRef(passDesc.pPass);
		}
	}
		
	SafeAddRef(m_pEffect);
}

HRESULT CShader::InitializePrototype(const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else 
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif	

	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	_uint iGetTechniqueIndex = 0;

	while (true) {
		ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(iGetTechniqueIndex);

		if (nullptr == pTechnique || !pTechnique->IsValid())
			break;

		D3DX11_TECHNIQUE_DESC techniqueDesc;
		ZeroMemory(&techniqueDesc, sizeof(D3DX11_TECHNIQUE_DESC));

		if (FAILED(pTechnique->GetDesc(&techniqueDesc)))
			return E_FAIL;

		vector<PASSDESC> passes;
		for (_uint i = 0; i < techniqueDesc.Passes; ++i) {
			PASSDESC passDesc;
			ZeroMemory(&passDesc, sizeof(PASSDESC));

			passDesc.pPass = pTechnique->GetPassByIndex(i);

			D3DX11_PASS_DESC passInfo;

			passDesc.pPass->GetDesc(&passInfo);

			if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements,
				passInfo.pIAInputSignature, passInfo.IAInputSignatureSize,
				&passDesc.pInputLayout)))
				return E_FAIL;

			passes.push_back(passDesc);
		}

		m_techniques.push_back(passes);

		++iGetTechniqueIndex;
	}

	return S_OK;
}

HRESULT CShader::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CShader::SetRawValue(const char* pContantName, const void* pData, _uint iByteLength)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 특정 이름을 가진 셰이더 전역변수의 핸들을 얻어온다. */	
	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pContantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iByteLength);	
}

HRESULT CShader::SetMatrixArray(const char * pConstantName, const _float4x4* pData, _uint iNumMatrices)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable*		pMatrixVariable = pVariable->AsMatrix();

	return pMatrixVariable->SetMatrixArray(&pData->_11, 0, iNumMatrices);
}

HRESULT CShader::SetShaderResourceView(const char * pContantName, ID3D11ShaderResourceView * pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pContantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pShaderResource = pVariable->AsShaderResource();
	if (nullptr == pShaderResource)
		return E_FAIL;

	return pShaderResource->SetResource(pSRV);
}

HRESULT CShader::SetShaderResourceViewArray(const char * pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTexture)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*		pShaderResourceVariable = pVariable->AsShaderResource();
	if (nullptr == pShaderResourceVariable)
		return E_FAIL;

	return pShaderResourceVariable->SetResourceArray(ppSRV, 0, iNumTexture);
}

HRESULT CShader::Begin(_uint iTechniqueIndex, _uint iPassIndex)
{
	if (iTechniqueIndex >= m_techniques.size() && iPassIndex >= m_techniques[iTechniqueIndex].size())
		return E_FAIL;

	/* 이러한 정보를 가진 정점을 그리낟. */
	/* 이러한 정보를 가진  : 이미 셰이더에서 받을 수 있음을 검증했다. */
	m_pContext->IASetInputLayout(m_techniques[iTechniqueIndex][iPassIndex].pInputLayout);

	/* 해당 ID3DXEffectPass로 그리낟. */
	/* Apply를 호출하기전에 모든 변수를 셰이더로 전달하는 역활을 마무리해야한다. */
	m_techniques[iTechniqueIndex][iPassIndex].pPass->Apply(0, m_pContext);

	return S_OK;
}

CShader * CShader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader*			pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(pShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX(TEXT("Failed To Created : CShader"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent * CShader::Clone(void * pArg)
{
	CShader*			pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShader"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& passes : m_techniques) {
		for (auto& passDesc : passes) {
			SafeRelease(passDesc.pInputLayout);
		}

		passes.clear();
	}

	m_techniques.clear();


	SafeRelease(m_pEffect);
}
