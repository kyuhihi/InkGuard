#pragma once

#include "Component.h"

/* ȭ�鿡 �׷������� ��ü���� �׷������� ������� ��Ƽ� �����Ѵ�.  */
/* �����ϰ� �ִ� ���b���� �����Լ��� ȣ�����ֳ�. */

BEGIN(Engine)
class CLight;
class ENGINE_DLL CRenderer final : public CComponent
{
public: /* �׷����� ������ ���� ���� */
	enum RENDERGROUP { 
		RENDER_PRIORITY, 
		RENDER_NONALPHABLEND, 
		RENDER_SHADOW_DEPTH,
		RENDER_NONLIGHT, 
		RENDER_ALPHABLEND, 
		RENDER_BACKBUFFER,
		RENDER_UI, 
		RENDER_END 
	};

	enum SHADER_TECHNIQUE {
		TECHNIQUE_DEFAULT,
		TECHNIQUE_SSD,
		TECHNIQUE_SHADOW,
		TECHNIQUE_END
	};
		
	enum DEFAULT_SHADER_TECHNIQUE {
		DEFAULT_DEFAULT,
		DEFAULT_DIRECTIONAL,
		DEFAULT_POINT,
		DEFAULT_BLEND,
		DEFAULT_BACKBUFFER,
		DEFAULT_END
	};

	enum SHADOW_SHADER_TECHNIQUE {
		SHADOW_SHADOW,
		SHADOW_BLEND,
		SHADOW_END
	};

	struct SSD {
		_float			fLiveTime = 1.f;
		class CTexture* pTexture = nullptr;
		_uint			iTextureIndex = 0;
		_uint			iSSDShaderPass = 0;
		_matrix			vObjectMatrix;
		_float2			vSize;
	};

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRenderer(const CRenderer& rhs);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT AddRenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);	
	HRESULT Draw();

#pragma region For.SSD
public:
	void SetTimeDelta(_float fTimeDelta) { m_fTimeDelta = fTimeDelta; };
	void AddSSD(const SSD tSSD) { m_SSDs.push_back(tSSD); };

private:
	void	MinusSSDTimes();

private:
	_float		m_fTimeDelta{ 0.f };
	list<SSD>	m_SSDs;
#pragma endregion

#ifdef _DEBUG
public:
	HRESULT AddDebugGroup(class CComponent* pDebugCom);
	HRESULT DebugDraw();
	HRESULT DebugDrawClear();
#endif // _DEBUG

private:
	HRESULT RenderPriority();
	HRESULT RenderNonAlphaBlend();
	HRESULT RenderLights();
	HRESULT RenderBlend();
	HRESULT RenderNonLight();
	HRESULT RenderAlphaBlend();
	HRESULT RenderUI();

	HRESULT RenderShadow();
	HRESULT RenderBackBuffer();

#ifdef _DEBUG
	HRESULT RenderDebug();
#endif

private:
	HRESULT RenderShadowDepth(CLight* pLight); // �� ������ ���̸� �׸��ϴ�.
	HRESULT RenderShadowColor(_matrix worldMatrix, CLight* pLight); // �׸��� ������ �׸��ϴ�.
	HRESULT RenderShadowBlend(_matrix worldMatrix, CLight* pLight); // �������� �׸��ڸ� ȭ�鿡 �׸��ϴ�.

private:
	ID3D11DepthStencilView* m_pShadowDepthStencil;
	D3D11_VIEWPORT m_pShadowDepthViewport;

private:
	using RENDEROBJECTS = list<class CGameObject*>;
	RENDEROBJECTS m_RenderObjects[RENDER_END];

#ifdef _DEBUG
private:
	list<class CComponent*> m_DebugObject;
#endif // _DEBUG

private:
	class CTargetManager* m_pTargetManager = nullptr;
	class CLightManager* m_pLightManager = nullptr;

	class CShader* m_pShader = nullptr;
	class CVIBufferRect* m_pVIBuffer = nullptr;

private:
	_float4x4 m_viewMatrix, m_projMatrix;

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END