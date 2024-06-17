#include "stdafx.h"
#include "..\Public\Glass.h"
#include "GameInstance.h"

CGlass::CGlass(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_fScale(1.8f)
	, m_fOffsetY(1.f)
{
}

CGlass::CGlass(const CGlass& rhs)
	: CGameObject(rhs)
	, m_fScale(rhs.m_fScale)
	, m_fOffsetY(rhs.m_fOffsetY)
{
}

HRESULT CGlass::InitializePrototype()
{
	return S_OK;
}

HRESULT CGlass::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->SetScale(XMVectorSet(m_fScale, m_fScale, m_fScale, 1.f));

	return S_OK;
}

void CGlass::Tick(_float fTimeDelta)
{

	if (m_pTargetTransform == nullptr)
	{
		m_pTargetTransform = (CTransform*)CGameInstance::GetInstance()->GetComponentPtr(m_iCurrentLevel, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	}
	//m_pTransformCom->EditTransform(true);
	_vector vPlayerPos = m_pTargetTransform->GetState(CTransform::STATE_POSITION);
	vPlayerPos.m128_f32[1] += m_fOffsetY;
	m_pTransformCom->SetState(CTransform::STATE_POSITION, vPlayerPos);
	//RenderIMGUI();


}

void CGlass::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;

	_vector vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->GetCamPosition());
	_vector vMyPos= m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_vector vBillBoardLook = XMVector3Normalize(vMyPos - vCamPos);
	m_pTransformCom->LookAt(XMVectorSetW(m_pTransformCom->GetState(CTransform::STATE_POSITION) + vBillBoardLook,1.f));

	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CGlass::Render(_uint eRenderGroup)
{
	if (nullptr == m_pVIBufferCom ||
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

	if (FAILED(m_pTextureCom->SetSRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	_float4 vMultipleColor = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pShaderCom->SetRawValue("g_fMultipleColor", &vMultipleColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0, 2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}



HRESULT CGlass::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::AddComponent(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Texture_InnerEmptyCircle"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

void CGlass::RenderIMGUI()
{
	ImGui::Begin("Glass");
	_float3 vPositon;
	XMStoreFloat3(&vPositon, m_pTransformCom->GetState(CTransform::STATE_POSITION));
	ImGui::Text("[Position]");
	ImGui::Text(to_string(vPositon.x).data()); ImGui::SameLine();
	ImGui::Text(to_string(vPositon.y).data()); ImGui::SameLine();
	ImGui::Text(to_string(vPositon.z).data());

	ImGui::End();
}

CGlass* CGlass::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGlass*		pInstance = new CGlass(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CGlass"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CGlass::Clone(void * pArg)
{
	CGlass*		pInstance = new CGlass(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CGlass"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CGlass::Free()
{
	__super::Free();

	SafeRelease(m_pTextureCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);

	m_pTargetTransform = nullptr;
}
