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
	//���� ����ǰ��ִ� �ִϸ��̼ǰ� ������ �ִϸ��̼��� �������� �н�
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

	/* ������ �����Ѵ�. */
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

	/* ���� ����ϰ����ϴ� �ִϸ��̼��� �����ؾ��� ������ ��������� �����س���. */
	m_bAnimEnd = m_Animations[m_iCurrentAnimIndex]->PlayAnimation(fTimeDelta);

	/* ��������� ����������(�θ𿡼� �ڽ�����) �����Ͽ� m_CombinedTransformation�� �����.  */
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

	/* ���� ����ϰ����ϴ� �ִϸ��̼��� �����ؾ��� ������ ��������� �����س���. */
	m_bAnimEnd = m_Animations[m_iCurrentAnimIndex]->PlayAnimation(fTimeDelta, EventName);

	/* ��������� ����������(�θ𿡼� �ڽ�����) �����Ͽ� m_CombinedTransformation�� �����.  */
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

		/* �� ������ ��Ű��. */
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
	{//��ŷ�� ģ������ ������
		if (iter->Picking(pTransform, pOut, m_pAIScene->mMeshes[iMeshCounter])) {
			vPickingPoints.push_back(*pOut);
			bPickingSuccess = true;
		}
		++iMeshCounter;
	}
	if (bPickingSuccess == true)
	{//��ŷ�� �����ߴ�.
		vector<_float> vecPoints;
		_vector vCamPosition = XMLoadFloat4(&CPipeLine::GetInstance()->GetCamPosition());
		for (auto& iter : vPickingPoints)
		{//��ŷ�� ģ����� ī�޶��� �Ÿ��� �����س���.
			_vector vPoints = XMVectorSetW(XMLoadFloat3(&iter), 1.f);
			vecPoints.push_back(abs(XMVector4Length(vPoints - vCamPosition).m128_f32[0]));
		}
		_float fMinPoint{ vecPoints.front() };
		_int iPointNum{ 0 }, iRemPoint{ 0 };
		for (auto& iter : vecPoints)
		{//������ �Ÿ����� ���ʷ� ��ȸ�ϸ� �� �Ÿ��� �� ������ �����Ѵ�.
			if (iter < fMinPoint) {
				fMinPoint = iter;
				iRemPoint = iPointNum;
			}
			++iPointNum;
		}
		*pOut = vPickingPoints[iRemPoint];//���� ��ŷ��ģ���� ��ŷ����Ʈ�� ��������.
		return true;
	}
	else
		return false;
}

HRESULT CModel::ReadyMeshContainers(_fmatrix PivotMatrix, _bool bPicking)
{
	/* �޽��� ������ ���´�. */
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

	/* �� ���� ��� ���׸��� ������ �̿��ϴ°�. */
	/* ���׸���(MATERIALDESC) : �ؽ���[��ǻ��or�ں��Ʈor�븻or�̹̽ú� ���� ] */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));

		aiMaterial*			pAIMaterial = m_pAIScene->mMaterials[i];

		/* AI_TEXTURE_TYPE_MAX:��ǻ��or�ں��Ʈor�븻or�̹̽ú� ���� */
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{			
			aiString		strPath;			

			/* �ش� ������ ǥ���ϱ����� �ؽ����� ��θ� strPath�� �޾ƿ´�. */
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
	/* pParent? : �θ� ��� �ּ�. CombinedTransformation���� �׸���. 
	CombinedTransformation���� ������� ���� Transformation * �θ���CombinedTranfsormation�� �����. */
	/* �׷��� �θ� �ʿ���. */
	/* iDepth? : ReadyHierarchyNodes�Լ��� ������·� �θ�����ֱ⤨�Ť̤��� ����(����)���� �����س����� ������. */
	/* ���� �ִԿ��Ӽ� ����Ҷ� ���� CombinedTransformation�� �����. */
	/* CombinedTransformation������� �θ��� ���°� ��� ���ŵǾ��־�ߵ�. �� �θ��� �Ĺ��ε� �̿��ϴϱ�.�� == 
	 �θ���� �ڽ����� ���������� CombinedTransformation�� �������Ѵٶ�� �� �ǹ�.  */
	/* m_HierarchyNodes�����̳ʴ� �ֻ��� �θ� ���� �տ� �־���Ѵ�. �̳��� 1�� �ڽĵ��� �ι�°�� ��. ����¥�ĵ��� �״����Ҿ�. */
	/* �� ��帶�� ���̰�(�����ڽ��̳�? ) �� �����صΰ� ���߿� �����Ѵ�. */
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

		/*I �ִϸ��̼� ���� ��üȭ �ϴ� ���� : ���� ��� �ð��� �´� ä�ε��� �� ���¸� �����Ѵ�. (���� ������) 
		�Լ��� ����������. */
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
