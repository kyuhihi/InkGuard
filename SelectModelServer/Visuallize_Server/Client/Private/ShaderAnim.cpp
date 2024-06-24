#include "stdafx.h"
#include "..\Public\ShaderAnim.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CShaderAnim::CShaderAnim(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CShaderAnim::CShaderAnim(const CShaderAnim & rhs)
	: CGameObject(rhs)
{
}

HRESULT CShaderAnim::InitializePrototype()
{
	return S_OK;
}

HRESULT CShaderAnim::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iShaderIndex = &m_iIndexForNullArg;

	if (nullptr != pArg)
		m_iShaderIndex = (_uint*)pArg;

	m_pModelCom->SetAnimIndex(3);

	m_pTransformCom->SetScale(XMVectorSet(1.f, 1.f, 1.f, 0.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(180.f));

	return S_OK;
}

void CShaderAnim::Tick(_float fTimeDelta)
{
	KeyInput(fTimeDelta);
}

void CShaderAnim::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pModelCom->PlayAnimation(fTimeDelta);

	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
	
#endif
}

HRESULT CShaderAnim::Render(_uint eRenderGroup)
{
	if (nullptr == m_pModelCom || nullptr == m_vShaderCom[*m_iShaderIndex])
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_vShaderCom[*m_iShaderIndex]->SetRawValue("g_WorldMatrix", &m_pTransformCom->GetWorldFloat4x4TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_vShaderCom[*m_iShaderIndex]->SetRawValue("g_ViewMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_vShaderCom[*m_iShaderIndex]->SetRawValue("g_ProjMatrix", &pGameInstance->GetTransformFloat4x4TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUpOnShader(m_vShaderCom[*m_iShaderIndex], m_pModelCom->GetMaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom[m_iShaderIndex], m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/

		if (FAILED(m_pModelCom->Render(m_vShaderCom[*m_iShaderIndex], i)))
			return E_FAIL;
	}


#ifdef _DEBUG
	
#endif

	return S_OK;
}

HRESULT CShaderAnim::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	CShader* pShaderCom = nullptr;

	/* For.Com_Shader */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&pShaderCom)))
		return E_FAIL;

	m_vShaderCom.push_back(pShaderCom);

	/* For.Com_Shader_LimLighting_Anim */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Shader_RimLighting_Anim"), TEXT("Com_Shader_Rim_Anim"), (CComponent**)&pShaderCom)))
	return E_FAIL;

	m_vShaderCom.push_back(pShaderCom);

	/* For.Com_Shader_Cartoon_Anim */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Cartoon_Anim"), TEXT("Com_Shader_Cartoon_Anim"), (CComponent**)&pShaderCom)))
		return E_FAIL;

	m_vShaderCom.push_back(pShaderCom);


	/* For.Com_Model */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Model_Fiona"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

void CShaderAnim::KeyInput(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Change_Animation_Index */
	/*static _uint iAnim = 0;
	static _uint iTemp = 0;

	if (pGameInstance->GetDIKDownState(DIK_UP))
		if (iTemp < 3) {
			iTemp++;
			m_pModelCom->ChangeAnimation(iAnim, iTemp, 0.2f);
		}

	if (pGameInstance->GetDIKDownState(DIK_DOWN))
		if (iTemp > 0) {
			iTemp--;
			m_pModelCom->ChangeAnimation(iAnim, iTemp, 0.2f);
		}*/

	/* For.Change_Shader_Component */
	/*if (pGameInstance->GetDIKDownState(DIK_NUMPAD0))
		m_iIndexForNullArg = SHADERMODEL::SHADER_ANIM;

	if (pGameInstance->GetDIKDownState(DIK_NUMPAD1))
		m_iIndexForNullArg = SHADERMODEL::SHADER_RIM_ANIM;

	if (pGameInstance->GetDIKDownState(DIK_NUMPAD2))
		m_iIndexForNullArg = SHADERMODEL::SHADER_CARTOON_ANIM;*/

	/* For.Turn_Model */
	if (pGameInstance->GetDIKState(DIK_LEFT))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

	if (pGameInstance->GetDIKState(DIK_RIGHT))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -fTimeDelta);

	RELEASE_INSTANCE(CGameInstance);
}

CShaderAnim * CShaderAnim::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShaderAnim* pInstance = new CShaderAnim(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CShaderAnim"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CShaderAnim::Clone(void * pArg)
{
	CShaderAnim* pInstance = new CShaderAnim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShaderAnim"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CShaderAnim::Free()
{
	__super::Free();

	for (auto& pShaderCom : m_vShaderCom)
		SafeRelease(pShaderCom);

	m_vShaderCom.clear();

	SafeRelease(m_pModelCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
}
