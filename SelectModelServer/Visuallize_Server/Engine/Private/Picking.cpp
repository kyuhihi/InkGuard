#include "..\Public\Picking.h"
#include "Transform.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}


HRESULT CPicking::Initialize(HWND hWnd, _uint iWinCX, _uint iWinCY)
{
	m_hWnd = hWnd;

	m_iWinCX = iWinCX;

	m_iWinCY = iWinCY;

	return S_OK;
}

void CPicking::Tick()
{
	POINT			ptMouse;

	GetCursorPos(&ptMouse);

	ScreenToClient(m_hWnd, &ptMouse);

	_float3			vMousePos;

	/* 투영공간상의 마우스 좌표를 구한다. */
	vMousePos.x = _float(ptMouse.x / (m_iWinCX * 0.5f) - 1);
	vMousePos.y = _float(ptMouse.y / (m_iWinCY * -0.5f) + 1);
	vMousePos.z = 0.f;

	/* vector -> float : XMStore*/
	/* float -> vector : XMLoad */

	/* 뷰스페이스 상의 좌표를 구한다. */
	_float4x4		ProjMatrix;
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	//m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
	
	ProjMatrix = pPipeLine->GetTransformFloat4x4(pPipeLine->D3DTS_PROJ);
	_matrix ProjMatrixinv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&ProjMatrix));
	//_vector vViewMouse = XMVector3TransformCoord(XMLoadFloat3(&vMousePos), XMLoadFloat4x4(&ProjMatrixInv));
	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), ProjMatrixinv));

	/* 뷰스페이스 상의 마우스 레이와 레이포스를 구하자. */	
	m_vRayPos = _float3(0.f, 0.f, 0.f);
	XMStoreFloat3(&m_vRayDir, XMLoadFloat3(&vMousePos) - XMLoadFloat3(&m_vRayPos));
	/* 월드스페이스 상의 마우스 레이와 레이포스를 구하자. */	

	_float4x4		ViewMatrix;
	ViewMatrix = pPipeLine->GetTransformFloat4x4(pPipeLine->D3DTS_VIEW);
	_matrix ViewMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&ViewMatrix));
	XMStoreFloat3(&m_vRayPos,XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos),  ViewMatrixInv));
	XMStoreFloat3(&m_vRayDir,XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), ViewMatrixInv));
	_int a = 0;
}


_vector CPicking::Compute_LocalRayInfo(_bool bPos, CTransform * pTransform)
{
	_matrix		WorldMatrixInv = pTransform->GetWorldMatrixInverse();

	_vector vTemp{};
	if (bPos) {
		 vTemp = XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), WorldMatrixInv);
	}
	else {
		 vTemp = XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), WorldMatrixInv);
	}
	return vTemp;

}

void CPicking::Free()
{
	
}
