#include "stdafx.h"
#include "..\Public\SkyStar.h"
#include "GameInstance.h"

CSkyStar::CSkyStar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkyStar::CSkyStar(const CSkyStar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkyStar::InitializePrototype()
{
	return S_OK;
}

HRESULT CSkyStar::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float3 fInitPos = *(_float3*)pArg;
	_vector vFixPos = XMVector3Normalize(XMLoadFloat3(&fInitPos))*70.f;
	XMStoreFloat3(&m_vStarPosition, vFixPos);
	m_pTransformCom->SetState(CTransform::STATE_POSITION, XMVectorSetW(vFixPos, 1.f));

	m_pTransformCom->SetScale(XMVectorSet(5.f,5.f, 5.f, 1.f));

	m_vAuroraSequence[AURORA_YELLOW] = XMVectorSet(141.f, 255.f, 0.f, 256.f);
	m_vAuroraSequence[AURORA_GREEN] = XMVectorSet(0.f, 184.f, 154.f, 256.f);
	m_vAuroraSequence[AURORA_ORANGE] = XMVectorSet(255.f, 57.f, 0.f, 256.f);


	random_device rd;
	default_random_engine dre{ rd() };
	uniform_int_distribution<_int> uid(AURORA_YELLOW, AURORA_ORANGE);

	m_iCurAuroraSequence = (_uint)uid(dre);

	random_device rdDisappear;
	default_random_engine dreDisappear{ rdDisappear() };
	uniform_int_distribution<_int> uidDisappear(1, 10);

	m_fDisappearMaxTime -= uidDisappear(dreDisappear) * 0.8f;
	m_fDisappearMaxTime = abs(m_fDisappearMaxTime);
	XMStoreFloat4(&m_vCurAuroraColor, m_vAuroraSequence[m_iCurAuroraSequence]);

	return S_OK;
}

void CSkyStar::Tick(_float fTimeDelta)
{
	if (CGameInstance::GetInstance()->isInFrustumWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION)))
		m_bCulling = false;
	else {
		m_bCulling = true;
		return;
	}
		
	
	m_fAlphaTime -=  0.0016f * m_fDisappearMaxTime*0.1f;
	if (m_fAlphaTime < 0.f){
		m_fAlphaTime = 1.f;
	}

	AuroraColorChangeTick(fTimeDelta);

}

void CSkyStar::LateTick(_float fTimeDelta)
{
	if (m_bCulling)
		return;
	if (nullptr == m_pRendererCom)
		return;

	_vector vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->GetCamPosition());
	_vector vFixStarPos = XMLoadFloat3(&m_vStarPosition) + vCamPos;
	m_pTransformCom->SetState(CTransform::STATE_POSITION, XMVectorSetW(vFixStarPos, 1.f));
	
	_vector vStarPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_vector vBillBoardLook = XMVector3Normalize(vStarPos- vCamPos);
	m_pTransformCom->LookAt(XMVectorSetW(m_pTransformCom->GetState(CTransform::STATE_POSITION) + vBillBoardLook,1.f));

	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CSkyStar::Render(_uint eRenderGroup)
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

	if (FAILED(m_pTextureCom->SetSRV(m_pShaderCom, "g_DiffuseTexture", 1)))
		return E_FAIL;

	_float fUVNoChange{ 0.f };
	if (FAILED(m_pShaderCom->SetRawValue("g_fNoiseU", &fUVNoChange, sizeof(_float))))
		return E_FAIL;


	_float fRGB = (1.f / 256.f);

	_float4 MakeAuroraColor{ fRGB * m_vCurAuroraColor.x,fRGB * m_vCurAuroraColor.y,fRGB * m_vCurAuroraColor.z,m_fAlphaTime };

	if (FAILED(m_pShaderCom->SetRawValue("g_fMultipleColor", &MakeAuroraColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0, 2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CSkyStar::AuroraColorChangeTick(_float fTimeDelta)
{
	_vector vNewColor = XMVectorLerp(m_vAuroraSequence[m_iCurAuroraSequence], m_vAuroraSequence[CheckArraySize(m_iCurAuroraSequence + 1)], m_fSequenceLerp);
	XMStoreFloat4(&m_vCurAuroraColor, vNewColor);
	m_fSequenceLerp += fTimeDelta * 0.1f;
	CheckAuroraFinish();
}

void CSkyStar::CheckAuroraFinish()
{
	if (m_fSequenceLerp >= 1.f) {
		++m_iCurAuroraSequence;
		m_fSequenceLerp = 0.f;

		if (m_iCurAuroraSequence == AURORA_END)
		{
			m_iCurAuroraSequence = AURORA_YELLOW;
		}
	}
}

_uint CSkyStar::CheckArraySize(_uint iSequence)
{
	if (iSequence == AURORA_END)
		return AURORA_YELLOW;
	return iSequence;
}

HRESULT CSkyStar::Ready_Components()
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
	if (FAILED(__super::AddComponent(m_iCurrentLevel, TEXT("Prototype_Component_Texture_SkyEffect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CSkyStar * CSkyStar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkyStar*		pInstance = new CSkyStar(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSkyStar"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CSkyStar::Clone(void * pArg)
{
	CSkyStar*		pInstance = new CSkyStar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSkyStar"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CSkyStar::Free()
{
	__super::Free();

	SafeRelease(m_pTextureCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
	
}
