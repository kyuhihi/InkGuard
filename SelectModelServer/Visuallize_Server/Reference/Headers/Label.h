#pragma once
#include "Component.h"

BEGIN(Engine)
class CShader;
class ENGINE_DLL CLabel final : public CComponent
{
public:
	enum FONT_ALIGN {
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER,
		ALIGN_JUSTIFIED,
		ALIGN_END
	};

	using FONT_DESC = struct tagFontDesc {
		const _tchar* name;
		_float fFontSize;
		_float2 vBoxSize;
		DWRITE_FONT_WEIGHT eFontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL;
		DWRITE_FONT_STYLE eFontStyle = DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL;
		DWRITE_FONT_STRETCH eFontStretch = DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL;
		_float4 vFontBackgroundColor = _float4(0.f, 0.f, 0.f, 0.f);
	};

private:
	CLabel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLabel(const CLabel& rhs);
	virtual ~CLabel() = default;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT SetSRV(CShader* pShader, const char* pConstantName);

	void UpdateFont(const _tchar* text, FONT_ALIGN align, _float4 vColor, DWRITE_PARAGRAPH_ALIGNMENT eParagraphAlign = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	void SetFont(const _tchar* pFontName);
	void SetFontSize(_float fFontSize);
	void SetFontWeight(DWRITE_FONT_WEIGHT eFontWeight);
	void SetStretch(DWRITE_FONT_STRETCH eFontStretch);
	void SetFontBackgroundColor(_float4 vColor) { m_tFontInfo.vFontBackgroundColor = vColor; };
	void SetBoxSize(_float2 vSize);

	void SetInnerSpacing(_float fSpacing) { m_fInnerSpacing = fSpacing; };

private:
	HRESULT CreateTextFormat();
	HRESULT CreateLabelD2DRenderTarget();

private:
	ID3D11ShaderResourceView* m_pSRV;

	IDWriteFactory* m_pIDWriteFactory;
	IDWriteTextFormat* m_pFormat;
	FONT_DESC m_tFontInfo;

	ID2D1RenderTarget* m_pRenderTarget;

	_float m_fInnerSpacing = 10.f;

public:
	static CLabel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
