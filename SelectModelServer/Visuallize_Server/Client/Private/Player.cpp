#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Light.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, WALK_SPEED(4.f)
	, RUN_SPEED(8.f)
	, PLAYER_CAM_DISTANCE(15.f)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
	, WALK_SPEED(rhs.WALK_SPEED)
	, RUN_SPEED(rhs.RUN_SPEED)
	, PLAYER_CAM_DISTANCE(rhs.PLAYER_CAM_DISTANCE)
{
}

HRESULT CPlayer::InitializePrototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_pTransformCom->SetScale(XMVectorSet(1.f, 1.f, 1.f, 0.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(180.f));
	
	m_iAnim[ANIMATION::ANIM_SLEEP] = 29;
	m_iAnim[ANIMATION::ANIM_RUN] = 25;


	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	m_pIntervalComponent->Update(fTimeDelta);

	RenderIMGUI();
}

void CPlayer::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	_float4 vLightPosition;
	XMStoreFloat4(&vLightPosition, m_pTransformCom->GetState(CTransform::STATE_POSITION));
	vLightPosition.y += 10.f;
	m_pLight->GetLightDesc()->vPosition = vLightPosition;

	if (m_bKeyPressed)
		m_pModelCom->SetAnimIndex(m_iAnim[ANIM_RUN]);
	else
		m_pModelCom->SetAnimIndex(m_iAnim[ANIM_SLEEP]);

	m_pModelCom->PlayAnimation(fTimeDelta);
	
	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_NONLIGHT, this);
	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_SHADOW_DEPTH, this);

}

HRESULT CPlayer::Render(_uint eRenderGroup)
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->SetRawValue("g_WorldMatrix", &m_pTransformCom->GetWorldFloat4x4TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_ViewMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_ProjMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{	
		if (ModelIndex::MODEL_KIND_EYE == i || ModelIndex::MODEL_KIND_EYE_AROUND == i || ModelIndex::MODEL_CLOSE_EYE == i)
			continue;

		if (FAILED(m_pModelCom->SetUpOnShader(m_pShaderCom, m_pModelCom->GetMaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUpOnShader(m_pShaderCom, m_pModelCom->GetMaterialIndex(i), aiTextureType_NORMALS, "g_DiffuseTexture"))) {
			ID3D11ShaderResourceView* pNullTexture = nullptr;
			m_pShaderCom->SetRawValue("g_NormalTexture", &pNullTexture, sizeof(ID3D11ShaderResourceView*));
		}

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}	


	return S_OK;
}

HRESULT CPlayer::RenderLightDepth(CLight* pLight) {

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (_uint i = 0; i < iNumMeshes; ++i) {
		if (FAILED(m_pShaderCom->SetRawValue("g_WorldMatrix",
			&m_pTransformCom->GetWorldFloat4x4TP(), sizeof(_float4x4))))
			return E_FAIL;

		_float4x4 lightViewMatrix = pLight->GetViewMatrixTP();
		if (FAILED(m_pShaderCom->SetRawValue("g_ViewMatrix",
			&lightViewMatrix, sizeof(_float4x4))))
			return E_FAIL;

		_float4x4 lightProjMatrix = pLight->GetProjectionMatrixTP();
		if (FAILED(m_pShaderCom->SetRawValue("g_ProjMatrix",
			&lightProjMatrix, sizeof(_float4x4))))
			return E_FAIL;

		_float fProjFar = pGameInstance->GetProjectionFar();
		if (FAILED(m_pShaderCom->SetRawValue("g_fProjFar", &fProjFar, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1, 0)))
			return E_FAIL;
	}

	m_bKeyPressed = false;

	return S_OK;
}

void CPlayer::RenderIMGUI()
{
	ImGui::Begin("Player");
	_float3 vPositon;
	XMStoreFloat3(&vPositon, m_pTransformCom->GetState(CTransform::STATE_POSITION));
	ImGui::Text("[Position]");
	ImGui::Text(to_string(vPositon.x).data()); ImGui::SameLine();
	ImGui::Text(to_string(vPositon.y).data()); ImGui::SameLine();
	ImGui::Text(to_string(vPositon.z).data());

	const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
	static int item_current = 1;
	ImGui::ListBox("listbox", &item_current, items, IM_ARRAYSIZE(items), 5);

	ImGui::End();
}

void CPlayer::KeyInput(CTransform* pCamTransform, const _float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->GetDIKState(DIK_A) & 0x80)
		pCamTransform->GoLeft(fTimeDelta);

	else if (pGameInstance->GetDIKState(DIK_D) & 0x80)
		pCamTransform->GoRight(fTimeDelta);
	


	_vector PlayerLocation = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_vector CamLocation = pCamTransform->GetState(CTransform::STATE_POSITION);
	
	_vector LookPlayerDir = XMVector4Normalize(PlayerLocation - CamLocation);
	_vector pCamLerpLocation = PlayerLocation  - LookPlayerDir * PLAYER_CAM_DISTANCE;
	pCamLerpLocation.m128_f32[1] = PlayerLocation.m128_f32[1] + 5.f;

	pCamTransform->SetState(CTransform::STATE_POSITION, XMVectorLerp(CamLocation, pCamLerpLocation, fTimeDelta*3.F));
	pCamTransform->LookAt(m_pTransformCom->GetState(CTransform::STATE_POSITION));

	//m_pTransformCom->LookAt_ForLandObject(PlayerLocation + pCamTransform->GetState(CTransform::STATE_LOOK));

	_float MoveValue = WALK_SPEED;

	if(pGameInstance->GetDIKState(DIK_LSHIFT) & 0x80)
		MoveValue = RUN_SPEED;

	if (pGameInstance->GetDIKState(DIK_W) & 0x80)
	{
		m_pTransformCom->LookAt_ForLandObject(PlayerLocation + pCamTransform->GetState(CTransform::STATE_LOOK));
		m_pTransformCom->GoStraight(MoveValue * fTimeDelta);
		m_bKeyPressed = true;
	}

	if (pGameInstance->GetDIKState(DIK_S) & 0x80)
	{
		m_pTransformCom->GoBackward(MoveValue * fTimeDelta);
		m_bKeyPressed = true;
	}


	if (pGameInstance->GetDIKState(DIK_Q) & 0x80)
	{
		m_pTransformCom->GoUp(MoveValue * fTimeDelta);
	}

	if (pGameInstance->GetDIKState(DIK_E) & 0x80)
	{
		m_pTransformCom->GoDown(MoveValue * fTimeDelta);
	}

}

HRESULT CPlayer::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = WALK_SPEED;
	TransformDesc.fRotationPerSec = XMConvertToRadians(180.f);

	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Interval */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Interval"), TEXT("Com_Interval"), (CComponent**)&m_pIntervalComponent)))
		return E_FAIL;
		
	/* For.Com_Shader */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Model_Player"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	LIGHTDESC			LightDesc;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.bShadow = true;
	LightDesc.vPosition = _float4(25.0f, 5.0f, 15.0f, 1.f);
	LightDesc.fRange = 100.f;
	LightDesc.vDiffuse = _float4(1.0f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	m_pGameInstance = CGameInstance::GetInstance();
	m_pLight = CGameInstance::GetInstance()->AddLight(m_pDevice, m_pContext, LightDesc);
	if (m_pLight == nullptr)
		return E_FAIL;

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	SafeRelease(m_pModelCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pIntervalComponent);
}
