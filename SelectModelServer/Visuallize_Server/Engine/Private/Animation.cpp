#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"
#include "HierarchyNode.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_Channels(rhs.m_Channels)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fPlayTime(rhs.m_fPlayTime)
{
	for (auto& pChannel : m_Channels)
		SafeAddRef(pChannel);
}

HRESULT CAnimation::InitializePrototype(aiAnimation * pAIAnimation)
{
	m_fDuration = static_cast<_float>(pAIAnimation->mDuration);
	m_fTickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);
	
	strcpy(m_szName, pAIAnimation->mName.C_Str());
	/* 현재 애니메이션에서 제어해야할 뼈들의 갯수를 저장한다. */
	m_iNumChannels = pAIAnimation->mNumChannels;


	/* 현재 애니메이션에서 제어해야할 뼈정보들을 생성하여 보관한다. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		/* aiNodeAnim : mChannel은 키프레임 정보들을 가지낟. */
		CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		/* 왜 모아두는데?> 특정 애님에ㅣ션 상태일때 애니메이션을 재생하면 모든 뼈의 상태를 갱신하는건 빡세. 느려. 
		현재 애미에시연을 구동하기위한 뼈대만 상태 갱신해주기 위해. */
		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(CModel* pModel)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		KEYFRAMEINFO KeyFrameInfo;
		KeyFrameInfo.iKeyFrame = 0;
		KeyFrameInfo.EventName.reserve(10);
		m_ChannelKeyFrames.push_back(KeyFrameInfo);

		CHierarchyNode*		pNode = pModel->GetHierarchyNode(m_Channels[i]->GetName());

		if (nullptr == pNode)
			return E_FAIL;		

		m_HierarchyNodes.push_back(pNode);
		
		SafeAddRef(pNode);
	}

	return S_OK;
}


_bool CAnimation::PlayAnimation(_float fTimeDelta)
{
	if (m_bBlending)
	{
		m_fPlayTime += fTimeDelta;

		_uint		iChannelIndex = 0;

		for (auto& pChannel : m_Channels)
		{
			pChannel->BlendingTransformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex].iKeyFrame, m_HierarchyNodes[iChannelIndex], m_fBlendTime);

			++iChannelIndex;
		}

		if (m_fPlayTime >= m_fBlendTime)
		{
			m_fPlayTime = 0.f;
			m_bBlending = false;
		}
		return false;
	}
	else
	{
		_bool bAnimEnd = false;
		m_fPlayTime += m_fTickPerSecond * fTimeDelta;

		if (m_fPlayTime >= m_fDuration)
		{
			if (m_bLoop)
			{
				m_fPlayTime = 0.f;
				for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
					iCurrentKeyFrame.iKeyFrame = 0;
			}
			bAnimEnd = true;
		}
		else
			bAnimEnd = false;


		//영향을 주는 뼈들을 갱신해줌
		/* 이 애니메이션 구동을 위한 모든 뼈들을 순회하며 뼈들의 행렬을 갱신해준다. */
		/* Transformation : 전달된 시간에 따른 키프레임(시간, 스케일, 회전, 이동)정보를 이용하여 Transformation을 만든다. */
		/* 하이어라키 노드에 저장해준다. */
		_uint		iChannelIndex = 0;

		for (auto& pChannel : m_Channels)
		{
			m_ChannelKeyFrames[iChannelIndex].iKeyFrame = pChannel->UpdateTransformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex].iKeyFrame, m_HierarchyNodes[iChannelIndex]);

			++iChannelIndex;
		}
		return bAnimEnd;
	}
}

_bool CAnimation::PlayAnimation(_float fTimeDelta, vector<string>* EventName)
{
	if (m_bBlending)
	{
		m_fPlayTime += fTimeDelta;

		_uint		iChannelIndex = 0;

		for (auto& pChannel : m_Channels)
		{
			pChannel->BlendingTransformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex].iKeyFrame, m_HierarchyNodes[iChannelIndex], m_fBlendTime);

			++iChannelIndex;
		}

		if (m_fPlayTime >= m_fBlendTime)
		{
			m_fPlayTime = 0.f;
			m_bBlending = false;
		}
		return false;
	}
	else
	{
		_bool bAnimEnd = false;
		m_fPlayTime += m_fTickPerSecond * fTimeDelta;

		if (m_fPlayTime >= m_fDuration)
		{
			if (m_bLoop)
			{
				m_fPlayTime = 0.f;
				for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
					iCurrentKeyFrame.iKeyFrame = 0;
			}
			bAnimEnd = true;
		}
		else
			bAnimEnd = false;


		//영향을 주는 뼈들을 갱신해줌
		/* 이 애니메이션 구동을 위한 모든 뼈들을 순회하며 뼈들의 행렬을 갱신해준다. */
		/* Transformation : 전달된 시간에 따른 키프레임(시간, 스케일, 회전, 이동)정보를 이용하여 Transformation을 만든다. */
		/* 하이어라키 노드에 저장해준다. */
		_uint		iChannelIndex = 0;
		
		_uint		tempCurrentFrame = m_iCurrentKeyFrame;
		//애니메이션 재생후 키프레임이 변경될것임
		for (auto& pChannel : m_Channels)
		{
			m_ChannelKeyFrames[iChannelIndex].iKeyFrame = pChannel->UpdateTransformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex].iKeyFrame, m_HierarchyNodes[iChannelIndex], tempCurrentFrame);

			++iChannelIndex;
		}

		if (m_iCurrentKeyFrame != tempCurrentFrame)
		{
			m_iCurrentKeyFrame++;
			if (m_iCurrentKeyFrame >= m_ChannelKeyFrames.size())
			{
				m_iCurrentKeyFrame = 0;
			}
			//키프레임이 변경되었다면 (다음으로 넘어갔다면 실행)
			for (_uint i = 0; i < m_ChannelKeyFrames[m_iCurrentKeyFrame].EventName.size(); ++i)
			{
				EventName->push_back(m_ChannelKeyFrames[m_iCurrentKeyFrame].EventName[i]);
			}
		}
		

		return bAnimEnd;
	}
}

void CAnimation::ChangeAnimation(CAnimation * pPrevAnimation)
{
	_uint		iChannelIndex = 0;

	for (auto& pChannel : m_Channels)
	{
		pChannel->SetOldMatrix(pPrevAnimation->GetChannel(iChannelIndex)->GetOldMatrix());

		++iChannelIndex;
	}

	m_fPlayTime = 0.f;
	m_bBlending = true;
}

void CAnimation::ResetKeyFrames()
{
	for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
	{
		iCurrentKeyFrame.iKeyFrame = 0;
	}
	m_bBlending = false;
	
	for (auto& pChannel : m_Channels)
	{
		pChannel->ResetOldMatrix();
	}
}

CChannel * CAnimation::GetChannel(_uint iChannelIndex)
{
	return  m_Channels[iChannelIndex];
}

_uint CAnimation::GetNumKeyFrame()
{
	// 어차피 모든 채널은 키프레임 개수가 똑같다. (로드할 때 맞춰주기 때문)
	// 그래서 첫번째 키프레임갯수만 가져옴
	return 	static_cast<_uint>(m_ChannelKeyFrames.size());
}

_uint CAnimation::GetCurrentKeyFrame()
{
	return m_iCurrentKeyFrame;
}

void CAnimation::SetKeyFrameEvent(_uint iKeyFrame, const char * EventName)
{
	if (iKeyFrame >= m_ChannelKeyFrames.size())
		return;
	m_ChannelKeyFrames[iKeyFrame].EventName.push_back(EventName);
}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation)
{
	CAnimation*			pInstance = new CAnimation();

	if (FAILED(pInstance->InitializePrototype(pAIAnimation)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone(CModel* pModel)
{
	CAnimation*			pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Initialize(pModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		SafeRelease(pChannel);

	m_Channels.clear();


	for (auto& pHierarchyNode : m_HierarchyNodes)
		SafeRelease(pHierarchyNode);

	m_HierarchyNodes.clear();
}
