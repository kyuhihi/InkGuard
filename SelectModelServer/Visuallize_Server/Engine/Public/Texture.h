#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT InitializePrototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT SetSRV(class CShader* pShader, const char* pConstantName, _uint iTextureIndex = 0);
	ID3D11ShaderResourceView* GetSRV(_uint iTextureIndex) {
		if (iTextureIndex >= m_SRVs.size())
			return nullptr;

		return m_SRVs[iTextureIndex];
	};

private:	
	/* 셰이더에 직접 바인딩 될 수 있는 객체 */
	using SRVS = vector<ID3D11ShaderResourceView*>;
	SRVS m_SRVs;

	_uint m_iNumTextures = 0;

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();
};

END