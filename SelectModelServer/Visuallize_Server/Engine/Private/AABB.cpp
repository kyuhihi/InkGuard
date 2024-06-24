#include "..\Public\AABB.h"
#include "PipeLine.h"

#include "OBB.h"
#include "Sphere.h"

CAABB::CAABB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCollider(pDevice, pContext)
{

}

CAABB::CAABB(const CAABB & rhs)
	: CCollider(rhs)
	, m_pAABB(rhs.m_pAABB)
{

}

HRESULT CAABB::InitializePrototype(CCollider::TYPE eColliderType)
{
	if (FAILED(__super::InitializePrototype(eColliderType)))
		return E_FAIL;
	m_eColliderType = CCollider::TYPE_AABB;
	return S_OK;
}

HRESULT CAABB::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	/* 복제될때 셋팅하고자하는 상태로 갱신한다. */
	m_pOriginalAABB = new BoundingBox(m_ColliderDesc.vCenter, _float3(m_ColliderDesc.vSize.x * 0.5f,
		m_ColliderDesc.vSize.y * 0.5f, 
		m_ColliderDesc.vSize.z * 0.5f));

	m_pAABB = new BoundingBox(*m_pOriginalAABB);

	return S_OK;
}

void CAABB::Update(_fmatrix TransformMatrix)
{
	m_pOriginalAABB->Transform(*m_pAABB, RemoveRotation(TransformMatrix));
}

_bool CAABB::Collision(CCollider * pTargetCollider)
{
	CCollider::TYPE		eType = pTargetCollider->GetColliderType();

	m_isColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isColl = m_pAABB->Intersects(((CAABB*)pTargetCollider)->GetCollider());
		break;

	case CCollider::TYPE_OBB:
		m_isColl = m_pAABB->Intersects(((COBB*)pTargetCollider)->GetCollider());
		break;

	case CCollider::TYPE_SPHERE:
		m_isColl = m_pAABB->Intersects(((CSphere*)pTargetCollider)->GetCollider());
		break;
	}	

	return m_isColl;
}

_bool CAABB::CollisionAABB(CCollider * pTargetCollider)
{
	if (CCollider::TYPE_AABB != pTargetCollider->GetColliderType())
		return false;

	CAABB*		pTargetAABB = (CAABB*)pTargetCollider;

	_float3		vSourMin, vSourMax;
	_float3		vDestMin, vDestMax;

	vSourMin = ComputeMin();
	vSourMax = ComputeMax();

	vDestMin = pTargetAABB->ComputeMin();
	vDestMax = pTargetAABB->ComputeMax();

	m_isColl = false;

	/* 너비로 겹쳤는지?! */
	if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
		return m_isColl;

	/* 높이로 */
	if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
		return m_isColl;

	/* 깊이로 */
	if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z))
		return m_isColl;

	m_isColl = true;

	return m_isColl;
}

#ifdef _DEBUG

HRESULT CAABB::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pBatch->Begin();
	
	DX::Draw(m_pBatch, *m_pAABB, XMLoadFloat4(&m_vColor));

	m_pBatch->End();

	return S_OK;
}

#endif // _DEBUG

_float3 CAABB::ComputeMin()
{
	return _float3(m_pAABB->Center.x - m_pAABB->Extents.x, 
		m_pAABB->Center.y - m_pAABB->Extents.y, 
		m_pAABB->Center.z - m_pAABB->Extents.z);
}

_float3 CAABB::ComputeMax()
{
	return _float3(m_pAABB->Center.x + m_pAABB->Extents.x,
		m_pAABB->Center.y + m_pAABB->Extents.y,
		m_pAABB->Center.z + m_pAABB->Extents.z);
}

CAABB * CAABB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CCollider::TYPE eColliderType)
{
	CAABB*			pInstance = new CAABB(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(eColliderType)))
	{
		MSG_BOX(TEXT("Failed To Created : CAABB"));
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CAABB::Clone(void * pArg)
{
	CAABB*			pInstance = new CAABB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAABB"));
		SafeRelease(pInstance);
	}
	return pInstance;
}


void CAABB::Free()
{
	__super::Free();

	SafeDelete(m_pOriginalAABB);
	SafeDelete(m_pAABB);
}
