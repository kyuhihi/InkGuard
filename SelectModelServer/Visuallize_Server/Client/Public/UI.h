#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBufferRect;
END

BEGIN(Client)

class CUI : public CGameObject
{//기본 투영좌표 유아이들의 부모이다.
public:
	struct UIInit{
		string sPrototypeName;
		_uint  iTexturePass;
	};

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint eRenderGroup) override;

public:
	const string	GetTextureName() { return m_sTextureName; }
	
	void			SetParent(class CGameObject* pParentObject) { m_ParentObject = pParentObject; };
	void			SetTextureNumber(_uint iTextureNum) { m_iTextureNum = iTextureNum; }
	

protected:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBufferRect*			m_pVIBufferCom = nullptr;

protected:
	_float					m_fX, m_fY;
	_float					m_fSizeX, m_fSizeY;	//사이즈조절
	_float					m_fPosX, m_fPosY;					//위치조절
	_float4x4				m_ViewMatrix, m_ProjMatrix;
	_uint					m_iRenderNum{ 0 };					//렌더되는 순서이다.
	_uint					m_iTextureNum{ 0 };

	string					m_sTextureName;
	class CGameObject*		m_ParentObject = nullptr;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END