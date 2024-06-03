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
	m_tInfo = *(DebugTerritoryStruct*)(pArg);
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

	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_SHADOW_DEPTH, this);
	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTerritory::Render(_uint eRenderGroup)
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->SetRawValue("g_WorldMatrix", &m_pTransformCom->GetWorldFloat4x4TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_ViewMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_ProjMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);



	_uint		iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{	
		if (FAILED(m_pModelCom->SetUpOnShader(m_pShaderCom, m_pModelCom->GetMaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->SetUpOnShader(m_pShaderCom, m_pModelCom->GetMaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}	

	return S_OK;
}

HRESULT CTerritory::RenderLightDepth(CLight* pLight) {
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

	return S_OK;
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

	
	switch (m_tInfo.eNewShape)
	{
	case SHAPE_CIRCLE:
		/* For.Com_Model */
		if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Model_Territory_Circle"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case SHAPE_RECT:
		/* For.Com_Model */
		if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Model_Territory_Rect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	default:
		break;
	}


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


	SafeRelease(m_pModelCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
}
