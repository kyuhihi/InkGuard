#include "..\Public\FontManager.h"
#include "CustomFont.h"

IMPLEMENT_SINGLETON(CFontManager)

CFontManager::CFontManager()
{
}

HRESULT CFontManager::AddFonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr != FindFonts(pFontTag))
		return E_FAIL;

	CCustomFont*		pFont = CCustomFont::Create(pDevice, pContext, pFontFilePath);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(pFontTag, pFont);

	return S_OK;
}

HRESULT CFontManager::RenderFonts(const _tchar * pFontTag, const _tchar * pText, _float2 vPosition, _fvector vColor, _float fAngle, _float2 vOrigin, _float2 vScale)
{
	CCustomFont*		pFont = FindFonts(pFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(pText, vPosition, vColor, fAngle, vOrigin, vScale);	
}

CCustomFont * CFontManager::FindFonts(const _tchar * pFontTag)
{
	auto	iter = find_if(m_Fonts.begin(), m_Fonts.end(), CTagFinder(pFontTag));

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;	
}

void CFontManager::Free()
{
	for (auto& Pair : m_Fonts)
		SafeRelease(Pair.second);

	m_Fonts.clear();
}
