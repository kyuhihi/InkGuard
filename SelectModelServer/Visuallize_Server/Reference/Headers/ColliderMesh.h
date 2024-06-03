#pragma once

#include "Collider.h"

BEGIN(Engine)
class ENGINE_DLL CColliderMesh final : public CCollider
{
public:
	using MESHDESC = struct tagMeshDesc
	{
		vector<_float3> vertices;
		vector<FACEINDICES32> indices;
		COLLIDERDESC colliderDesc;
		_bool bConvex = false;
	};
private:
	CColliderMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CColliderMesh(const CColliderMesh& rhs);
	virtual ~CColliderMesh() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);

	virtual void Update(_fmatrix TransformMatrix) override;
	virtual _bool Collision(CCollider* pTargetCollider) override;
	virtual void SetSize(_float3 vSize) override;
	virtual void AttachShape() override;
	virtual PxTransform GetTransform() override;
	virtual PxRigidActor* GetActor() override;

private:
	PxRigidStatic* m_pBody = NULL;

public:
	static CColliderMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};
END
