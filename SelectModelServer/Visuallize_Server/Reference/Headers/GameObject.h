#pragma	 once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

protected:
	HRESULT AddComponent(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	HRESULT RemoveComponent(const _tchar* pComponentTag);
	HRESULT ComputeCamZ(_fvector vWorldPos);

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint eRenderGroup);
	virtual HRESULT RenderLightDepth(class CLight* pLight);


public:
	_float GetCamDistance() const {
		return m_fCamDistance;
	}

	void SetCurrentLevel(_uint iLevel) { m_iCurrentLevel = iLevel; }
	
	void Destroy() { m_bDestroyed = true; };
	const _bool GetDestroyed() const { return m_bDestroyed; }
	
	const _bool GetVisible() const { return m_bVisible; }
	void SetVisible(_bool bVisible) { m_bVisible = bVisible; }
	void Hidden() { m_bVisible = false; }
	void Show() { m_bVisible = true; }

	const _bool GetActive() const { return m_bActive; }
	void Disable() { m_bActive = false; };
	void Activate() { m_bActive = true; };


public:
	class CComponent* GetComponentPtr(const _tchar* pComponentTag);
	virtual HRESULT SetUpState(_fmatrix StateMatrix) { return S_OK; }

private:
	class CComponent* FindComponent(const _tchar* pComponentTag);

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

protected: /* 객체에게 추가된 컴포넌트들을 키로 분류하여 보관한다. */
	using COMPONENTS = map<const _tchar*, class CComponent*>;
	COMPONENTS m_Components;
	_uint m_iCurrentLevel;
	_float m_fCamDistance = 0.f;

	_bool m_bVisible; // Render 여부
	_bool m_bActive; // 활성상태

private:
	_bool m_bDestroyed;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END