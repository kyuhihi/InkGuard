#include "stdafx.h"
#include "..\Public\ModelSky.h"
#include "GameInstance.h"

CModelSky::CModelSky(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CModelSky::CModelSky(const CModelSky & rhs)
	: CGameObject(rhs)
{
}

HRESULT CModelSky::InitializePrototype()
{
	return S_OK;
}

HRESULT CModelSky::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_pTransformCom->SetScale(XMVectorSet(0.00005f, 0.00002f, 0.00005f, 1.f));
	memcpy(m_fBlueColor, m_fDayBlueColor, sizeof(m_fDayBlueColor));
	return S_OK;
}

void CModelSky::Tick(_float fTimeDelta)
{
	m_fSkyUV += (fTimeDelta * 0.001f);

}

void CModelSky::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_vector vCamPosition = XMLoadFloat4(&pGameInstance->GetCamPosition());
	vCamPosition.m128_f32[1] -= 0.1f;
	m_pTransformCom->SetState(CTransform::STATE_POSITION, vCamPosition);
	
	RELEASE_INSTANCE(CGameInstance);

	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CModelSky::Render(_uint eRenderGroup)
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUpOnShader(m_pShaderCom, m_pModelCom->GetMaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2, m_eSkyPass)))
			return E_FAIL;
	}

	_matrix SkyReverseZ = m_pTransformCom->GetWorldMatrix();
	_vector vCamPos  = SkyReverseZ.r[3];
	SkyReverseZ *= XMMatrixRotationZ(XMConvertToRadians(180.f));
	
	SkyReverseZ.r[3] = vCamPos;
	SkyReverseZ.r[3].m128_f32[1] -= 0.5f;
	SkyReverseZ =XMMatrixTranspose(SkyReverseZ);
	_float4x4 FixSkyFloat4x4;
	XMStoreFloat4x4(&FixSkyFloat4x4, SkyReverseZ);
	if (FAILED(m_pShaderCom->SetRawValue("g_WorldMatrix", &FixSkyFloat4x4, sizeof(_float4x4))))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUpOnShader(m_pShaderCom, m_pModelCom->GetMaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2, m_eSkyPass)))
			return E_FAIL;
	}

	return S_OK;
}

void CModelSky::SetNight()
{
	m_eSkyPass = SKY_PASS::PASS_NIGHT;
	memcpy(m_fBlueColor, m_fNightBlueColor, sizeof(m_fBlueColor));
}

HRESULT CModelSky::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);

	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Model_ModelSky"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CModelSky::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->SetRawValue("g_WorldMatrix", &m_pTransformCom->GetWorldFloat4x4TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_ViewMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_ProjMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_fSkyU", &m_fSkyUV, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_SkyColor", &m_fBlueColor, sizeof(m_fBlueColor))))
		return E_FAIL;
	
	_float fGamma = 2.2f;
	if (FAILED(m_pShaderCom->SetRawValue("g_Gamma", &fGamma, sizeof(_float))))
		return E_FAIL; // gamma correction test

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CModelSky * CModelSky::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CModelSky*		pInstance = new CModelSky(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CModelSky"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CModelSky::Clone(void * pArg)
{
	CModelSky*		pInstance = new CModelSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModelSky"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CModelSky::Free()
{
	__super::Free();
	SafeRelease(m_pModelCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
}
