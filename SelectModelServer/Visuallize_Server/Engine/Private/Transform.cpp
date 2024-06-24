#include "..\Public\Transform.h"
#include "PipeLine.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{

}

void CTransform::SetState(STATE eState, _fvector vState)
{
	_matrix	WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	WorldMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

void CTransform::EditTransform(_bool editTransformDecomposition)
{
	/*void EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)*/
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	
	_float WorldMatrix[16];
	memcpy(WorldMatrix, &m_WorldMatrix, sizeof(_float4x4));

	_float CameraView[16];
	memcpy(CameraView, &pPipeLine->GetTransformFloat4x4(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));

	_float CameraProjection[16];
	memcpy(CameraProjection, &pPipeLine->GetTransformFloat4x4(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));

	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	if (editTransformDecomposition)
	{
		//if (ImGui::IsKeyPressed(90))
		//	mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		//if (ImGui::IsKeyPressed(69))
		//	mCurrentGizmoOperation = ImGuizmo::ROTATE;
		//if (ImGui::IsKeyPressed(82)) // r Key
		//	mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
			mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(WorldMatrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation);
		ImGui::InputFloat3("Rt", matrixRotation);
		ImGui::InputFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, WorldMatrix);

		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}
		/*if (ImGui::IsKeyPressed(83))
			useSnap = !useSnap;*/

		ImGui::Checkbox("##UseSnap", &useSnap);
		ImGui::SameLine();

		switch (mCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Scale Snap", &snap[0]);
			break;
		}
		ImGui::Checkbox("Bound Sizing", &boundSizing);
		if (boundSizing)
		{
			ImGui::PushID(3);
			ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
			ImGui::SameLine();
			ImGui::InputFloat3("Snap", boundsSnap);
			ImGui::PopID();
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;

	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	/*_float IdentityMatrix[16] = 
	{
		1.f,0.f,0.f,0.f,
		0.f,1.f,0.f,0.f,
		0.f,0.f,1.f,0.f,
		0.f,0.f,0.f,1.f
	};
	ImGuizmo::DrawGrid(CameraView, CameraProjection, IdentityMatrix, 100.f);
	ImGuizmo::DrawCubes(CameraView, CameraProjection, WorldMatrix, 1);*/


	ImGuizmo::Manipulate(CameraView, CameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, WorldMatrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	memcpy(&m_WorldMatrix, &WorldMatrix, sizeof(_float4x4));

	_float camDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&pPipeLine->GetCamPosition()) - GetState(CTransform::STATE_POSITION)));

	ImGuizmo::ViewManipulate(CameraView, 8.f, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

	RELEASE_INSTANCE(CPipeLine);

}

HRESULT CTransform::InitializePrototype()
{
	/* vector -> float : XMStore*/
	/* float -> vector : XMLoad */

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());	

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

void CTransform::GoStraight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = GetState(CTransform::STATE_POSITION);
	_vector		vLook = GetState(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	/* 프렐이어가 움직이고 난 이후의 위치를 네비게이션에 전달하여. */
	/* 현재 상황에서 움직일 수 있늕지 체크한다. */
	_bool		isMove = true;

	if (nullptr != pNavigation)
		isMove = pNavigation->isMove(vPosition);

	if(true == isMove)
		SetState(CTransform::STATE_POSITION, vPosition);
}

void CTransform::GoBackward(_float fTimeDelta)
{
	_vector		vPosition = GetState(CTransform::STATE_POSITION);
	_vector		vLook = GetState(CTransform::STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	SetState(CTransform::STATE_POSITION, vPosition);
}

void CTransform::GoLeft(_float fTimeDelta)
{
	_vector		vPosition = GetState(CTransform::STATE_POSITION);
	_vector		vRight = GetState(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	SetState(CTransform::STATE_POSITION, vPosition);
}

void CTransform::GoRight(_float fTimeDelta)
{
	_vector		vPosition = GetState(CTransform::STATE_POSITION);
	_vector		vRight = GetState(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	SetState(CTransform::STATE_POSITION, vPosition);
}

void CTransform::GoUp(_float fTimeDelta)
{
	_vector		vPosition = GetState(CTransform::STATE_POSITION);
	_vector		vUp= GetState(CTransform::STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	SetState(CTransform::STATE_POSITION, vPosition);
}

void CTransform::GoDown(_float fTimeDelta)
{
	_vector		vPosition = GetState(CTransform::STATE_POSITION);
	_vector		vDown = GetState(CTransform::STATE_UP);

	vPosition -= XMVector3Normalize(vDown) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	SetState(CTransform::STATE_POSITION, vPosition);
}

void CTransform::SetScale(_fvector vScaleInfo)
{
	SetState(CTransform::STATE_RIGHT, 
		XMVector3Normalize(GetState(CTransform::STATE_RIGHT)) * XMVectorGetX(vScaleInfo));
	SetState(CTransform::STATE_UP, 
		XMVector3Normalize(GetState(CTransform::STATE_UP)) * XMVectorGetY(vScaleInfo));
	SetState(CTransform::STATE_LOOK, 
		XMVector3Normalize(GetState(CTransform::STATE_LOOK)) * XMVectorGetZ(vScaleInfo));	
}

_float3 CTransform::GetScale()
{
	return _float3(
		XMVectorGetX(XMVector3Length(GetState(CTransform::STATE_RIGHT))),
		XMVectorGetX(XMVector3Length(GetState(CTransform::STATE_UP))),
		XMVectorGetX(XMVector3Length(GetState(CTransform::STATE_LOOK))));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);
	
	SetState(CTransform::STATE_RIGHT, XMVector3TransformNormal(GetState(CTransform::STATE_RIGHT), RotationMatrix));
	SetState(CTransform::STATE_UP, XMVector3TransformNormal(GetState(CTransform::STATE_UP), RotationMatrix));
	SetState(CTransform::STATE_LOOK, XMVector3TransformNormal(GetState(CTransform::STATE_LOOK), RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	// Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(60.0f));

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_float3		Scale = GetScale();

	SetState(CTransform::STATE_RIGHT, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f) * Scale.x, RotationMatrix));
	SetState(CTransform::STATE_UP, XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f) * Scale.y, RotationMatrix));
	SetState(CTransform::STATE_LOOK, XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f) * Scale.z, RotationMatrix));
}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vLook = vAt - GetState(CTransform::STATE_POSITION);

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = GetScale();

	SetState(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	SetState(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	SetState(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_ForLandObject(_fvector vAt)
{
	_vector		vLook = vAt - GetState(CTransform::STATE_POSITION);

	


	_float3		vScale = GetScale();

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;	

	vLook = XMVector3Normalize(XMVector3Cross(vRight, GetState(CTransform::STATE_UP))) * vScale.z;

	SetState(CTransform::STATE_RIGHT, vRight);
	SetState(CTransform::STATE_LOOK, vLook);

}

void CTransform::Move(_fvector vTargetPos, _float fTimeDelta, _float fLimitDistance)
{
	_vector		vPosition = GetState(CTransform::STATE_POSITION);
	_vector		vDirection = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDirection));

	if(fDistance > fLimitDistance)
		vPosition += XMVector3Normalize(vDirection) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	SetState(CTransform::STATE_POSITION, vPosition);
}

void CTransform::MoveToSlowOut(_fvector vTargetPosition, _float fTimeDelta)
{
	_vector vPosition = GetState(STATE_POSITION);
	_vector vDirection = vTargetPosition - vPosition;

	vPosition += vDirection * m_TransformDesc.fSpeedPerSec  * fTimeDelta;

	SetState(CTransform::STATE_POSITION, vPosition);
}

CTransform* CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTransform"));
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent* CTransform::Clone(void * pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTransform"));
		SafeRelease(pInstance);
	}

	return pInstance;
}


void CTransform::Free()
{
	__super::Free();
}
