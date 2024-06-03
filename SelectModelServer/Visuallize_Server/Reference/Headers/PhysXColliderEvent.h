#pragma once
#include "Base.h"

BEGIN(Engine)
class CPhysXManager;
class CPhysXColliderEvent : public PxSimulationEventCallback, public CBase
{
private:
	CPhysXColliderEvent(CPhysXManager* pPhysXManager);
	virtual ~CPhysXColliderEvent() = default;

public:
	void onConstraintBreak(PxConstraintInfo* pConstraints, PxU32 iCount) override;
	void onWake(PxActor** ppActors, PxU32 iCount) override;
	void onSleep(PxActor** ppActors, PxU32 iCount) override;
	void onTrigger(PxTriggerPair* pPairs, PxU32 iCount) override;
	void onAdvance(const PxRigidBody* const* pRigidBody, const PxTransform* pTransform, const PxU32 iCount) override;
	void onContact(const PxContactPairHeader& pPairHeader, const PxContactPair* pPairs, PxU32 iCount) override;
	bool isTriggerShape(PxShape* pShape);

// 콜백 내에서 SDK 상태를 수정하면 안 됩니다. 특히 객체를 생성하거나 파괴해서는 안 됩니다. 상태 수정이 필요한 경우 변경 사항을 버퍼에 저장하고 시뮬레이션 단계 후에 수행해야 합니다.
// onAdvance() 는 시뮬레이션이 실행되는 동안 호출

private:
	CPhysXManager* m_pPhysXManager;

public:
	static CPhysXColliderEvent* Create(CPhysXManager* pPhysXManager);
	virtual void Free() override;
};
END
