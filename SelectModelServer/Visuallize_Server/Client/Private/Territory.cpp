#include "stdafx.h"
#include "..\Public\Territory.h"
#include "GameInstance.h"
#include "Light.h"

CTerritory::CTerritory(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerritory::CTerritory(const CTerritory & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerritory::InitializePrototype()
{
	return S_OK;
}

HRESULT CTerritory::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CTerritory::Tick(_float fTimeDelta)
{

}

void CTerritory::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	RenderIMGUI();

	Modify_Transform();
	

	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_SHADOW_DEPTH, this);
	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTerritory::Render(_uint eRenderGroup)
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (FAILED(m_pShaderCom->SetRawValue("g_ViewMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_ProjMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pCurrentModel->GetNumMeshes();

	for (int i = 0; i < 4; i++) {

		if (FAILED(m_pShaderCom->SetRawValue("g_WorldMatrix", &m_pTransformCom->GetWorldFloat4x4TP(), sizeof(_float4x4))))
			return E_FAIL;

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pCurrentModel->SetUpOnShader(m_pShaderCom, m_pCurrentModel->GetMaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
			if (FAILED(m_pCurrentModel->SetUpOnShader(m_pShaderCom, m_pCurrentModel->GetMaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
				return E_FAIL;

			if (FAILED(m_pCurrentModel->Render(m_pShaderCom, i)))
				return E_FAIL;
		}
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f * i));
	}


	RELEASE_INSTANCE(CGameInstance);



	return S_OK;
}

HRESULT CTerritory::RenderLightDepth(CLight* pLight) {
	_uint iNumMeshes = m_pCurrentModel->GetNumMeshes();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

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

	for (int j = 0; j < 4; j++) {

		for (_uint i = 0; i < iNumMeshes; ++i) {

			if (FAILED(m_pShaderCom->SetRawValue("g_WorldMatrix",
				&m_pTransformCom->GetWorldFloat4x4TP(), sizeof(_float4x4))))
				return E_FAIL;

			if (FAILED(m_pCurrentModel->Render(m_pShaderCom, i, 1, 0)))
				return E_FAIL;
		}		
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f * j));

	}

	return S_OK;
}

void CTerritory::RenderIMGUI()
{
	ImGui::Begin("Utils");
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Territory");

	const char* items[] = { "A", "B", "C"};
	ImGui::ListBox("Territory Site", &m_iCurrentSelected, items, IM_ARRAYSIZE(items), 3);

	ImGui::End();
}

void CTerritory::Modify_Transform()
{
	if (m_iCurrentSelected == 1) {
		m_pCurrentModel = m_pModelCom_Rect;
		m_pTransformCom->SetScale(XMVectorSet(1.f, 1.f, 1.f, 1.f));
	}
	else {
		m_pCurrentModel = m_pModelCom_Circle;
		m_pTransformCom->SetScale(XMVectorSet(1.5f, 1.5f, 1.5f, 1.5f));
	}



}


HRESULT CTerritory::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Model_Territory_Circle"), TEXT("Com_Model_Circle"), (CComponent**)&m_pModelCom_Circle)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Model_Territory_Rect"), TEXT("Com_Model_Rect"), (CComponent**)&m_pModelCom_Rect)))
		return E_FAIL;

	return S_OK;
}

CTerritory * CTerritory::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerritory*		pInstance = new CTerritory(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTerritory"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CTerritory::Clone(void * pArg)
{
	CTerritory*		pInstance = new CTerritory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTerritory"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CTerritory::Free()
{
	__super::Free();

	SafeRelease(m_pModelCom_Rect);
	SafeRelease(m_pModelCom_Circle);
	m_pCurrentModel = nullptr;

	SafeRelease(m_pShaderCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
}
