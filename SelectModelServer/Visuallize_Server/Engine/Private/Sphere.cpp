#include "..\Public\Sphere.h"
#include "AABB.h"
#include "OBB.h"

CSphere::CSphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCollider(pDevice, pContext)
{

}

CSphere::CSphere(const CSphere & rhs)
	: CCollider(rhs)
{

}

HRESULT CSphere::InitializePrototype(CCollider::TYPE eColliderType)
{
	if (FAILED(__super::InitializePrototype(eColliderType)))
		return E_FAIL;

	m_eColliderType = CCollider::TYPE_SPHERE;

	return S_OK;
}

HRESULT CSphere::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pOriginalSphere = new BoundingSphere(m_ColliderDesc.vCenter, m_ColliderDesc.vSize.x);

	m_pSphere = new BoundingSphere(*m_pOriginalSphere);

	return S_OK;
}

void CSphere::Update(_fmatrix TransformMatrix)
{
	m_pOriginalSphere->Transform(*m_pSphere, TransformMatrix);
}

_bool CSphere::Collision(CCollider * pTargetCollider)
{
	CCollider::TYPE		eType = pTargetCollider->GetColliderType();

	m_isColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isColl = m_pSphere->Intersects(((CAABB*)pTargetCollider)->GetCollider());
		break;

	case CCollider::TYPE_OBB:
		m_isColl = m_pSphere->Intersects(((COBB*)pTargetCollider)->GetCollider());
		break;

	case CCollider::TYPE_SPHERE:
		m_isColl = m_pSphere->Intersects(((CSphere*)pTargetCollider)->GetCollider());
		break;
	}

	return m_isColl;
}

#ifdef _DEBUG
HRESULT CSphere::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_pSphere, XMLoadFloat4(&m_vColor));

	m_pBatch->End();


	return S_OK;
}
#endif // _DEBUG


CSphere * CSphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CCollider::TYPE eColliderType)
{
	CSphere*			pInstance = new CSphere(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(eColliderType)))
	{
		MSG_BOX(TEXT("Failed To Created : CSphere"));
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CSphere::Clone(void * pArg)
{
	CSphere*			pInstance = new CSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSphere"));
		SafeRelease(pInstance);
	}
	return pInstance;
}


void CSphere::Free()
{
	__super::Free();

	SafeDelete(m_pOriginalSphere);
	SafeDelete(m_pSphere);

}
