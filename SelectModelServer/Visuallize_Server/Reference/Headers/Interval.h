#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CInterval final : public CComponent
{
public:
	enum TYPE {
		TYPE_REPEAT,
		TYPE_ONE,
		TYPE_END
	};

	using INTERVAL = struct tagInterval
	{
		TYPE eType;
		_bool bRunning;
		_float fFrame;
		_float fRepeatSecond;
		_bool bDestroy;

		function<void()> pExcuteCallBack;

		tagInterval()
			: fFrame(0.f)
			, pExcuteCallBack(nullptr)
			, bRunning(true)
			, bDestroy(false)
		{

		}
	};

private:
	CInterval(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInterval(const CInterval& rhs);
	virtual ~CInterval() = default;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	void Update(_float fTimeDelta);
	HRESULT AddInterval(const _tchar* pTag, std::function<void()> call, _float fStartTime); // 한번만 실행됨
	HRESULT AddRepeatInterval(const _tchar* pTag, std::function<void()> call, _float fRepeatSecond); // 반복 타이머
	INTERVAL* FindInterval(const _tchar* pTag);
	HRESULT RestartInterval(const _tchar* pTag);
	HRESULT RemoveInterval(const _tchar* pTag);
	HRESULT StopInterval(const _tchar* pTag);
	HRESULT StopIntervalAll();
	HRESULT ClearInterval();

private:
	using INTERVALMAP = map<const _tchar*, INTERVAL*>;
	INTERVALMAP m_pIntervals;

public:
	static CInterval* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END