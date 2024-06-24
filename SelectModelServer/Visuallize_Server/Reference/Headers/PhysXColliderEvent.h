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

// �ݹ� ������ SDK ���¸� �����ϸ� �� �˴ϴ�. Ư�� ��ü�� �����ϰų� �ı��ؼ��� �� �˴ϴ�. ���� ������ �ʿ��� ��� ���� ������ ���ۿ� �����ϰ� �ùķ��̼� �ܰ� �Ŀ� �����ؾ� �մϴ�.
// onAdvance() �� �ùķ��̼��� ����Ǵ� ���� ȣ��

private:
	CPhysXManager* m_pPhysXManager;

public:
	static CPhysXColliderEvent* Create(CPhysXManager* pPhysXManager);
	virtual void Free() override;
};
END
