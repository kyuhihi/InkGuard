#include "stdafx.h"

#include "..\Public\LevelGamePlay.h"

/* loading resource */
#include "ForkLift.h"
#include "ParticleRect.h"
#include "ParticlePoint.h"
#include "CameraFree.h"
#include "Effect.h"
#include "Sky.h"
#include "Ground.h"
#include "Territory.h"
#include "SkyStar.h"
#include "ModelSky.h"
#include "Glass.h"
#include "Deferred.h"

#include "GameInstance.h"
#include "CameraFree.h"

CLevelGamePlay::CLevelGamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevelGamePlay::Loading(_int eLevelID)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	SafeAddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

	/* 개ㅑㄱ체원형 로드한다. */

	/* For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCameraFree::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Ground */
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Ground"),
		CGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ForkLift */
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_ForkLift"),
		CForkLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	
	/* For.Prototype_GameObject_Particle_Rect */
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Particle_Rect"),
		CParticleRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Particle_Point*/
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Particle_Point"),
		CParticlePoint::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect */
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Effect"),
		CEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect */
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Territory"),
		CTerritory::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_SkyStar"),
		CSkyStar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_ModelSky"),
		CModelSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Glass"),
		CGlass::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Effect */
	if (FAILED(pGameInstance->AddPrototype(TEXT("Prototype_GameObject_Deferred"),
		CDeferred::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* 텍스쳐를 로드한다. */
	

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain*/
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.jpg"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SkyEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/SkyEffect/SkyEffect_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InnerEmptyCircle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/rc_ringglow0.dds"), 1))))
		return E_FAIL;
	
	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* 모델를 로드한다. */

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBufferTerrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect_Instance */
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		CVIBufferRectInstance::Create(m_pDevice, m_pContext, 10))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point_Instance */
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		CVIBufferPointInstance::Create(m_pDevice, m_pContext, 10))))
		return E_FAIL;

	_matrix		PivotMatrix = XMMatrixIdentity();


	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Kuribo/", "Kuribo.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/ForkLift/", "ForkLift.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Territory_Circle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/InkGuard/Circle/", "SM_Plains_Floors_Block03_Mod.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Territory_Rect"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/InkGuard/Rect/", "SM_Plains_Floors_Block01_Mod.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Ground/", "Ground.fbx", PivotMatrix))))
		return E_FAIL;

	_matrix SkyMat = XMMatrixScaling(0.001f, 0.001f, 0.001f);
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ModelSky"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/SkyModel/","SkyBlueA.fbx"))))
		return E_FAIL;



	///* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBufferCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Terrain */
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Terrain"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model */
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Cube */
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Cube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_RectInstance*/
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_RectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE_DECLARATION::Elements, VTXRECTINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_PointInstance*/
	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_PointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE_DECLARATION::Elements, VTXPOINTINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));


	if (FAILED(pGameInstance->AddPrototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Cartoon"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cartoon.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	SafeRelease(pGameInstance);

	return __super::Loading(eLevelID);
}

HRESULT CLevelGamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;
	ReadyStarPos();

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

void CLevelGamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CLevelGamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CLevelGamePlay::ReadyStarPos()
{
	string InputJsonPath = "../Bin/Data/SkyStar.json";

	ifstream ifs(InputJsonPath);
	IStreamWrapper isStreamWrapper = ifs;

	Document doc;
	doc.ParseStream(isStreamWrapper);
	StringBuffer buffer;
	PrettyWriter<StringBuffer> write(buffer);
	doc.Accept(write);

	Value& Parent = doc["StarPos"];
	if (Parent.IsArray())
	{
		for (SizeType i = 0; i < Parent.Size(); ++i)
		{
			Value& v = Parent[i];
			_float3 vGoombaPos{ v["X"].GetFloat(),v["Y"].GetFloat(),v["Z"].GetFloat() };
			m_StarPosList.push_back(vGoombaPos);
		}
	}
}

HRESULT CLevelGamePlay::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.bShadow = false;
	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.f, -1.f, 0.f, 0.f);
	LightDesc.vDiffuse = _float4(0.5, 0.5, 0.5, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if ( nullptr == pGameInstance->AddLight(m_pDevice, m_pContext, LightDesc))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevelGamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	SafeAddRef(pGameInstance);

	CCamera::CAMERADESC			CameraDesc;

	CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.0f;

	CameraDesc.TransformDesc.fSpeedPerSec = 40.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(100.0f);
	CCameraFree* pCamera =(CCameraFree*)pGameInstance->AddGameObjectToLayer(TEXT("Prototype_GameObject_Camera_Free"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc);
	if (!pCamera)
		return E_FAIL;
	pCamera->SetPlayer(m_pPlayer);

	if (!pGameInstance->AddGameObjectToLayer(TEXT("Prototype_GameObject_Deferred"), LEVEL_GAMEPLAY, TEXT("Layer_Deferred")))
		return E_FAIL;

	SafeRelease(pGameInstance);
	return S_OK;
}

HRESULT CLevelGamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	SafeAddRef(pGameInstance);
	m_pPlayer = (CPlayer*)pGameInstance->AddGameObjectToLayer(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, pLayerTag);
	if (!m_pPlayer)
		return E_FAIL;
	pGameInstance->AddGameObjectToLayer(TEXT("Prototype_GameObject_Glass"), LEVEL_GAMEPLAY, pLayerTag);



	SafeRelease(pGameInstance);

	return S_OK;
}

HRESULT CLevelGamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	SafeAddRef(pGameInstance);


	CModelSky* pModelSky = (CModelSky*)pGameInstance->AddGameObjectToLayer(TEXT("Prototype_GameObject_ModelSky"), LEVEL_GAMEPLAY, pLayerTag);
	pModelSky->SetNight();

	Ready_Layer_SkyStar(TEXT(""));
	

	if (!pGameInstance->AddGameObjectToLayer(TEXT("Prototype_GameObject_Ground"), LEVEL_GAMEPLAY, pLayerTag))
		return E_FAIL;

	
	if (!pGameInstance->AddGameObjectToLayer(TEXT("Prototype_GameObject_Territory"), LEVEL_GAMEPLAY, pLayerTag))
		return E_FAIL;
	

	SafeRelease(pGameInstance);

	return S_OK;
}

HRESULT CLevelGamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	SafeAddRef(pGameInstance);

	SafeRelease(pGameInstance);



	return S_OK;
}

HRESULT CLevelGamePlay::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	SafeAddRef(pGameInstance);
	
	//if (!pGameInstance->AddGameObjectToLayer(TEXT("Prototype_GameObject_Particle_Rect"), LEVEL_GAMEPLAY, pLayerTag))
	//	return E_FAIL;

	if (!pGameInstance->AddGameObjectToLayer(TEXT("Prototype_GameObject_Particle_Point"), LEVEL_GAMEPLAY, pLayerTag))
		return E_FAIL;

	SafeRelease(pGameInstance);


	return S_OK;
}

HRESULT CLevelGamePlay::Ready_Layer_SkyStar(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CGameObject* pObject = nullptr;

	for (const auto& iter : m_StarPosList)
	{
		_float3 vStarPos{ iter };
		pObject = pGameInstance->AddGameObjectToLayer(TEXT("Prototype_GameObject_SkyStar"), LEVEL_GAMEPLAY, TEXT("Layer_Stars"), &vStarPos);
		if (!pObject)
			return E_FAIL;
	}

	
	return S_OK;
}

CLevelGamePlay * CLevelGamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevelGamePlay*		pInstance = new CLevelGamePlay(pDevice, pContext);

	return pInstance;
}

void CLevelGamePlay::Free()
{
	__super::Free();

}


