#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Shader.h"
#include "PipeLine.h"
#include "Picking.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_eModelType(rhs.m_eModelType)
	/*, m_HierarchyNodes(rhs.m_HierarchyNodes)*/
	, m_Animations(rhs.m_Animations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_bAnimEnd(false)
	, m_AnimationsName(rhs.m_AnimationsName)
	
{
	for (auto& pMeshContainer : m_Meshes)
		SafeAddRef(pMeshContainer);


	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			SafeAddRef(Material.pTexture[i]);
	}

	for (auto& pAnimation : m_Animations)
		SafeAddRef(pAnimation);

}

CHierarchyNode * CModel::GetHierarchyNode(const char * pNodeName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode) 
	{
		return !strcmp(pNodeName, pNode->GetName());
	});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;	
}

_uint CModel::GetMaterialIndex(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->GetMaterialIndex();
}

void CModel::ChangeAnimation(_uint iNextAnimIndex, _float fBlendTime, _bool bLoop)
{
	//현재 진행되고있는 애니메이션과 변경할 애니메이션이 같은경우는 패스
	if (m_iCurrentAnimIndex == iNextAnimIndex)
		return;

	_uint iPrevAnimIndex = m_iCurrentAnimIndex;
	m_iCurrentAnimIndex = iNextAnimIndex;

	m_Animations[m_iCurrentAnimIndex]->ResetKeyFrames();
	m_Animations[m_iCurrentAnimIndex]->SetBlendTimeAndLoop(fBlendTime, bLoop);
	m_Animations[m_iCurrentAnimIndex]->ChangeAnimation(m_Animations[iPrevAnimIndex]);
	m_Animations[iPrevAnimIndex]->ResetKeyFrames();
}

_uint CModel::GetNumAnims()
{
	if (!m_Animations.empty())
		return (_uint)m_Animations.size();
	else
		return 0;
}

_uint CModel::GetNumKeyFrame(_uint iAnimIndex)
{
	return 	m_Animations[iAnimIndex]->GetNumKeyFrame();
}

void CModel::ShowHierarchy()
{
	static int item_current_idx = 0;
	if (ImGui::BeginListBox("HierarchyList(Engine)"))
	{
		for (int n = 0; n < m_HierarchyNodes.size(); n++)
		{
			
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable((*m_HierarchyNodes[n]).GetName(), is_selected))
				item_current_idx = n;

			if (is_selected)
			{
				//ImGui::SetItemDefaultFocus();


				for (_uint i = 0; i < m_HierarchyNodes[n]->GetDepth(); i++)
				{
					if (ImGui::TreeNode((*m_HierarchyNodes[n]).GetName()))
					{
						/*if (ImGui::Selectable((*m_HierarchyNodes[n]).Get_Name(), is_selected))
						item_current_idx = n;*/

						ImGui::TreePop();
					}
				}
			}
		}
		ImGui::EndListBox();
	}
}

void CModel::SetKeyFrameEvent(_uint iAnimIndex, _uint iKeyFrame, const char * EventName)
{
	if (iAnimIndex >= m_Animations.size())
		return;
	m_Animations[iAnimIndex]->SetKeyFrameEvent(iKeyFrame, EventName);
}

_uint CModel::GetCurrentKeyFrame(_uint iAnimIndex)
{
	return 	m_Animations[iAnimIndex]->GetCurrentKeyFrame();
}

HRESULT CModel::InitializePrototype(TYPE eType, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix, _bool bPicking)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	_uint		iFlag = 0;

	m_eModelType = eType;

	if (TYPE_NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
	else
		iFlag |= aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;

	m_pAIScene = m_Importer.ReadFile(szFullPath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;	

	if (FAILED(ReadyMeshContainers(PivotMatrix, bPicking)))
		return E_FAIL;

	if (FAILED(ReadyMaterials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(ReadyAnimations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	ReadyHierarchyNodes(m_pAIScene->mRootNode, nullptr, 0);

	/* 뎁스로 정렬한다. */
	/*sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->GetDepth() < pDest->GetDepth();
	});*/

	if (TYPE_ANIM == m_eModelType)
	{
		_uint		iNumMeshes = 0;

		vector<CMeshContainer*>		MeshContainers;

		for (auto& pPrototype : m_Meshes)
		{
			CMeshContainer*		pMeshContainer = (CMeshContainer*)pPrototype->Clone();
			if (nullptr == pMeshContainer)
				return E_FAIL;

			MeshContainers.push_back(pMeshContainer);

			SafeRelease(pPrototype);
		}

		m_Meshes.clear();

		m_Meshes = MeshContainers;

		for (auto& pMeshContainer : m_Meshes)
		{
			if (nullptr != pMeshContainer)
				pMeshContainer->SetUpHierarchyNodes(this, m_pAIScene->mMeshes[iNumMeshes++]);
		}
	}

	vector<CAnimation*>		Animations;

	for (auto& pPrototype : m_Animations)
	{
		CAnimation*		pAnimation = pPrototype->Clone(this);
		if (nullptr == pAnimation)
			return E_FAIL;

		Animations.push_back(pAnimation);

		SafeRelease(pPrototype);
	}

	m_Animations.clear();

	m_Animations = Animations;

	return S_OK;
}

HRESULT CModel::SetUpOnShader(CShader * pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char * pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (m_Materials[iMaterialIndex].pTexture[eTextureType] == nullptr)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->SetSRV(pShader, pConstantName);	
}

HRESULT CModel::PlayAnimation(_float fTimeDelta)
{
	if (!m_bPlay)
		return S_OK;

	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	/* 현재 재생하고자하는 애니메이션이 제어해야할 뼈들의 지역행렬을 갱신해낸다. */
	m_bAnimEnd = m_Animations[m_iCurrentAnimIndex]->PlayAnimation(fTimeDelta);

	/* 지역행렬을 순차적으로(부모에서 자식으로) 누적하여 m_CombinedTransformation를 만든다.  */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->SetCombinedTransformation();
	}

	return S_OK;
}

HRESULT CModel::PlayAnimation(_float fTimeDelta, vector<string>* EventName)
{
	if (!m_bPlay)
		return S_OK;

	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	/* 현재 재생하고자하는 애니메이션이 제어해야할 뼈들의 지역행렬을 갱신해낸다. */
	m_bAnimEnd = m_Animations[m_iCurrentAnimIndex]->PlayAnimation(fTimeDelta, EventName);

	/* 지역행렬을 순차적으로(부모에서 자식으로) 누적하여 m_CombinedTransformation를 만든다.  */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->SetCombinedTransformation();
	}

	return S_OK;
}

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, _uint iTechnique, _uint iPassIndex)
{
	_float4x4		BoneMatrices[256];

	if (TYPE_ANIM == m_eModelType) 
	{
		m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

		/* 모델 정점의 스키닝. */
		if (FAILED(pShader->SetRawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 256)))
			return E_FAIL;
	}

	pShader->Begin(iTechnique, iPassIndex);
	
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

_bool CModel::Picking(CTransform * pTransform, _float3 * pOut)
{
	_uint iMeshCounter{ 0 };
	vector<_float3> vPickingPoints;
	_bool bPickingSuccess{ false };
	for (auto& iter : m_Meshes)
	{//피킹된 친구들을 모은다
		if (iter->Picking(pTransform, pOut, m_pAIScene->mMeshes[iMeshCounter])) {
			vPickingPoints.push_back(*pOut);
			bPickingSuccess = true;
		}
		++iMeshCounter;
	}
	if (bPickingSuccess == true)
	{//피킹이 성공했다.
		vector<_float> vecPoints;
		_vector vCamPosition = XMLoadFloat4(&CPipeLine::GetInstance()->GetCamPosition());
		for (auto& iter : vPickingPoints)
		{//피킹된 친구들과 카메라의 거리를 산출해낸다.
			_vector vPoints = XMVectorSetW(XMLoadFloat3(&iter), 1.f);
			vecPoints.push_back(abs(XMVector4Length(vPoints - vCamPosition).m128_f32[0]));
		}
		_float fMinPoint{ vecPoints.front() };
		_int iPointNum{ 0 }, iRemPoint{ 0 };
		for (auto& iter : vecPoints)
		{//산출한 거리들을 차례로 순회하며 그 거리가 더 작으면 저장한다.
			if (iter < fMinPoint) {
				fMinPoint = iter;
				iRemPoint = iPointNum;
			}
			++iPointNum;
		}
		*pOut = vPickingPoints[iRemPoint];//이제 피킹된친구의 피킹포인트를 내보낸다.
		return true;
	}
	else
		return false;
}

HRESULT CModel::ReadyMeshContainers(_fmatrix PivotMatrix, _bool bPicking)
{
	/* 메시의 갯수를 얻어온다. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], this, PivotMatrix, bPicking);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::ReadyMaterials(const char* pModelFilePath)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	/* 이 모델은 몇개의 머테리얼 정보를 이용하는가. */
	/* 머테리얼(MATERIALDESC) : 텍스쳐[디퓨즈or앰비언트or노말or이미시브 등등등 ] */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));

		aiMaterial*			pAIMaterial = m_pAIScene->mMaterials[i];

		/* AI_TEXTURE_TYPE_MAX:디퓨즈or앰비언트or노말or이미시브 등등등 */
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{			
			aiString		strPath;			

			/* 해당 재질을 표현하기위한 텍스쳐의 경로를 strPath에 받아온다. */
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;		

			char			szFullPath[MAX_PATH] = "";
			char			szFileName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar			szWideFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, static_cast<_int>(strlen(szFullPath)), szWideFullPath, MAX_PATH);


			MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szWideFullPath);
			if (nullptr == MaterialDesc.pTexture[j])
				return E_FAIL;			
		}	

		m_Materials.push_back(MaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::ReadyHierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth)
{
	/* pParent? : 부모 노드 주소. CombinedTransformation으로 그린다. 
	CombinedTransformation놈을 만들려면 나의 Transformation * 부모의CombinedTranfsormation로 만든다. */
	/* 그래서 부모가 필요해. */
	/* iDepth? : ReadyHierarchyNodes함수를 재귀형태로 부르고ㅓ있기ㄸ매ㅜㄴ에 한쪽(깊이)으로 생성해나가기 때문에. */
	/* 이후 애님에ㅣ션 재생할때 뼈의 CombinedTransformation를 만든다. */
	/* CombinedTransformation만들려면 부모의 상태가 모두 갱신되어있어야돼. 왜 부모의 컴바인드 이용하니까.ㄴ == 
	 부모부터 자식으로 순차적으로 CombinedTransformation를 만들어야한다라는 걸 의미.  */
	/* m_HierarchyNodes컨테이너는 최상위 부모가 가장 앞에 있어야한다. 이놈의 1차 자식들이 두번째에 쫙. 삼차짜식들이 그다음쫘악. */
	/* 각 노드마다 깊이값(몇차자식이냐? ) 을 저장해두고 나중에 정렬한다. */
	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode, pParent, iDepth++);

	if (nullptr == pHierarchyNode)
		return E_FAIL;

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		ReadyHierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth);
	}	

	return S_OK;
}

HRESULT CModel::ReadyAnimations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_pAIScene->mNumAnimations; ++i)
	{
		aiAnimation*		pAIAnimation = m_pAIScene->mAnimations[i];

		/*I 애니메이션 마다 객체화 하는 이유 : 현재 재생 시간에 맞는 채널들의 뼈 상태를 셋팅한다. (조난 빡세다) 
		함수로 만들어야지뭐. */
		CAnimation*			pAnimation = CAnimation::Create(pAIAnimation);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
		m_AnimationsName.push_back(pAnimation->GetName());
	}	
	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix,_bool bPicking)
{
	CModel*			pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(eType, pModelFilePath, pModelFileName, PivotMatrix, bPicking)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*			pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_HierarchyNodes)
		SafeRelease(pHierarchyNode);

	m_HierarchyNodes.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			SafeRelease(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMeshContainer : m_Meshes)
		SafeRelease(pMeshContainer);

	m_Meshes.clear();

	for (auto& pAnimation : m_Animations)
		SafeRelease(pAnimation);

	m_Animations.clear();

	m_AnimationsName.clear();

	m_Importer.FreeScene();
}
