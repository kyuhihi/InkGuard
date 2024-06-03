#pragma once

#include "Base.h"

BEGIN(Engine)

class CFontManager final : public CBase
{
	DECLARE_SINGLETON(CFontManager)
public:
	CFontManager();
	virtual ~CFontManager() = default;

public:
	HRESULT AddFonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT RenderFonts(const _tchar* pFontTag, const _tchar* pTextm, _float2 vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		_float fAngle = 0.f, _float2 vOrigin = _float2(0.f, 0.f), _float2 vScale = _float2(1.f, 1.f));

private:
	using FONTS = map<const _tchar*, class CCustomFont*>;
	FONTS m_Fonts;

private:
	class CCustomFont* FindFonts(const _tchar* pFontTag);

public:
	virtual void Free() override;
};

END