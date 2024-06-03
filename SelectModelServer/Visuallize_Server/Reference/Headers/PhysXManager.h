#pragma once
#include "Base.h"

BEGIN(Engine)
class CPhysXColliderEvent;
class CPhysXFilterCallback;
class ENGINE_DLL CPhysXManager final : public CBase
{
	DECLARE_SINGLETON(CPhysXManager)
public:
	using RAYCASTHIT = struct RaycastHit
	{
		class CCollider* pCollider = nullptr;
		_float3 vPoint;
		_float3 vNormal;
		_float fDistance;
	};
private:
	CPhysXManager();
	virtual ~CPhysXManager() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);

public:
	void InitializeScene();
	void Clear();
	static PxFilterFlags ShaderFilter(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
			PxFilterObjectAttributes attributes1, PxFilterData filterData1,
			PxPairFlags& pairFlags, const void* pConstantBlock, PxU32 iConstantBlockSize);

public:
	PxScene* GetScene() { return m_pScene; }
	PxPhysics* GetPhysics() { return m_pPhysics; }
	PxCooking* GetCooking()  { return m_pCooking; }
	PxControllerManager* GetControllerManager()  { return m_pControllerManager; }
	const _float GetSimultateTimeDelta() const { return m_pSimulateTimeDelta; }

	void AddEvnetCall(function<void()> pEvent);

private:
	void UpdateObjects();
	void EventCall();

private:
	PxDefaultAllocator m_pDefaultAllocatorCallback;
	PxDefaultErrorCallback m_pDefaultErrorCallback;
	PxDefaultCpuDispatcher* m_pDefaultCpuDispatcher = NULL;
	PxControllerManager* m_pControllerManager = NULL;
	PxTolerancesScale m_pTolerancesScale;
	PxFoundation* m_pFoundation = NULL;
	PxPhysics* m_pPhysics = NULL;
	PxScene* m_pScene = NULL;
#ifdef _DEBUG
	PxPvd* m_pPvd = NULL;
#endif // _DEBUG


	/*gpu*/
	PxCudaContextManager* m_pCudaContextManager = NULL;

	PxCooking* m_pCooking = NULL; // 사용자 정의 물체 생성 도움 클래스?

	CPhysXColliderEvent* m_pColliderEvent;
	CPhysXFilterCallback* m_pTriggersFilterCallback;

private:
	_float m_pSimulateTimeDelta;
	vector<function<void()>> m_pEventCallbacks;

public:
	virtual void Free() override;
};
END