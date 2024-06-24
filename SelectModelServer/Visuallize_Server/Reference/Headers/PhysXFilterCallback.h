#pragma once
#include "Base.h"

BEGIN(Engine)
class CPhysXFilterCallback : public PxSimulationFilterCallback, public CBase
{
private:
	CPhysXFilterCallback();
	virtual ~CPhysXFilterCallback() = default;
public:
	virtual PxFilterFlags pairFound(PxU32 iPairID,
		PxFilterObjectAttributes attributes0, PxFilterData filterData0, const PxActor* pActor0, const PxShape* pShape0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1, const PxActor* pActor1, const PxShape* pShape1,
		PxPairFlags& pairFlags) override;

	virtual	void pairLost(PxU32 iPairID,
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		bool bObjectRemoved);

	virtual bool statusChange(PxU32& iPairID, PxPairFlags& pairFlags, PxFilterFlags& filterFlags);

public:
	static CPhysXFilterCallback* Create();
	virtual void Free() override;
};
END