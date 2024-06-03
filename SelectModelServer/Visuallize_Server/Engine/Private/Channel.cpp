#include "..\Public\Channel.h"
#include "Model.h"
#include "HierarchyNode.h"



CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(aiNodeAnim * pAIChannel)
{
	/* Ư�� �ִϸ��̼ǿ��� ���Ǵ� ���� �����̴�. */
	/* �� �̸��� ���� ������ �ִ� HierarchyNodes�� ���� �� �ѳ�� �̸��� ���� ���̴�. */
	/* �� �̸����� ���� �̸��� ���� HierarchyNodes�� ä�ο� �����صд�. */
	/* �� �����ϴ�? : ä���� ���� Ű�������� �ð��뿡 �´� Ű�ÿ��Ӥ� ���¸� �����. �̰ɷ� ��� �����. 
	�̷��� ���� ����� erarchyNodes�� �����س�����. */
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	//m_pHierarchyNode = pModel->GetHierarchyNode(m_szName);
	//if (nullptr == m_pHierarchyNode)
	//	return E_FAIL;

	// SafeAddRef(m_pHierarchyNode);


	/* Ű������ �����鸦 �ε��Ѵ�. */
	/* Ű������ : ��ü�ִϸ��̼� ���� ��, Ư�� �ð��뿡 �� �̻��� ǥ���ؾ��� ������ ���� ��������̴�. */

	/* �� Ű������ ������ ũ��, ȸ��, �̵����� ������ �ٸ� �� �ִ�. */
	/* ������ �ٸ���? : �����ѳ��� ���� �ð��뿡 Ű�����Ӥ� ���¸� ������ �����̴�. */
	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	/* �� ������ ���� �ۿ� ����������? ���� ������ ��� �����س��� 
	���� ������ ���¸� �̿��ؼ� Ű������ ���¸� ����� ����. */
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME			KeyFrame;
		ZeroMemory(&KeyFrame, sizeof(KEYFRAME));

		if(i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = static_cast<_float>(pAIChannel->mScalingKeys[i].mTime);
		}
		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTime = static_cast<_float>(pAIChannel->mRotationKeys[i].mTime);
		}
		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = static_cast<_float>(pAIChannel->mPositionKeys[i].mTime);
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame);
	}	

	return S_OK;
}

_uint CChannel::UpdateTransformation(_float fPlayTime, _uint iCurrentKeyFrame, CHierarchyNode* pNode)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	/* ������ Ű�������̻����� �Ѿ���� : ������ Ű������ �ڼ��� ������ �� �ֵ��� �Ѵ�. */
	if (fPlayTime >= m_KeyFrames.back().fTime)
	{
		vScale = m_KeyFrames.back().vScale;
		vRotation = m_KeyFrames.back().vRotation;
		vPosition = m_KeyFrames.back().vPosition;		
	}

	/* Ư�� Ű�����Ӱ� Ű������ ���̿� �����Ѵ�. */
	/* 1�� �������� �󸶳� ����Ǿ�����(Ű�����Ӱ� Ű������ ���̸�)�� Ȯ���Ѵ�.( Ratio) */
	/* ������Ratio���� ���� �� Ű�����ӻ����� ��������� ����� ����. */
	else
	{
		while (fPlayTime >= m_KeyFrames[iCurrentKeyFrame + 1].fTime)
			++iCurrentKeyFrame;

		_float		fRatio = (fPlayTime - m_KeyFrames[iCurrentKeyFrame].fTime) /
			(m_KeyFrames[iCurrentKeyFrame + 1].fTime - m_KeyFrames[iCurrentKeyFrame].fTime);

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = m_KeyFrames[iCurrentKeyFrame].vScale;
		vDestScale = m_KeyFrames[iCurrentKeyFrame + 1].vScale;

		vSourRotation = m_KeyFrames[iCurrentKeyFrame].vRotation;
		vDestRotation = m_KeyFrames[iCurrentKeyFrame + 1].vRotation;

		vSourPosition = m_KeyFrames[iCurrentKeyFrame].vPosition;
		vDestPosition = m_KeyFrames[iCurrentKeyFrame + 1].vPosition;

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));	

	XMStoreFloat4x4(&m_OldMatrix, TransformationMatrix);

	if (nullptr != pNode)
		pNode->SetTransformation(TransformationMatrix);

	m_iCurrentKeyFrame = iCurrentKeyFrame;

	return m_iCurrentKeyFrame;
}

_uint CChannel::UpdateTransformation(_float fPlayTime, _uint iCurrentKeyFrame, CHierarchyNode * pNode, _uint & iAnimKeyFrame)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	/* ������ Ű�������̻����� �Ѿ���� : ������ Ű������ �ڼ��� ������ �� �ֵ��� �Ѵ�. */
	if (fPlayTime >= m_KeyFrames.back().fTime)
	{
		vScale = m_KeyFrames.back().vScale;
		vRotation = m_KeyFrames.back().vRotation;
		vPosition = m_KeyFrames.back().vPosition;
	}

	/* Ư�� Ű�����Ӱ� Ű������ ���̿� �����Ѵ�. */
	/* 1�� �������� �󸶳� ����Ǿ�����(Ű�����Ӱ� Ű������ ���̸�)�� Ȯ���Ѵ�.( Ratio) */
	/* ������Ratio���� ���� �� Ű�����ӻ����� ��������� ����� ����. */
	else
	{
		while (fPlayTime >= m_KeyFrames[iCurrentKeyFrame + 1].fTime)
		{
			++iCurrentKeyFrame;
			++iAnimKeyFrame;
		}

		_float		fRatio = (fPlayTime - m_KeyFrames[iCurrentKeyFrame].fTime) /
			(m_KeyFrames[iCurrentKeyFrame + 1].fTime - m_KeyFrames[iCurrentKeyFrame].fTime);

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = m_KeyFrames[iCurrentKeyFrame].vScale;
		vDestScale = m_KeyFrames[iCurrentKeyFrame + 1].vScale;

		vSourRotation = m_KeyFrames[iCurrentKeyFrame].vRotation;
		vDestRotation = m_KeyFrames[iCurrentKeyFrame + 1].vRotation;

		vSourPosition = m_KeyFrames[iCurrentKeyFrame].vPosition;
		vDestPosition = m_KeyFrames[iCurrentKeyFrame + 1].vPosition;

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));

	XMStoreFloat4x4(&m_OldMatrix, TransformationMatrix);

	if (nullptr != pNode)
		pNode->SetTransformation(TransformationMatrix);

	m_iCurrentKeyFrame = iCurrentKeyFrame;

	return m_iCurrentKeyFrame;
}

_uint CChannel::BlendingTransformation(_float fPlayTime, _uint iCurrentKeyFrame, CHierarchyNode * pNode, _float fBlendTime)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	_float		fRatio = fPlayTime / fBlendTime;

	if (fPlayTime <= fBlendTime)
	{
		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		_vector tempScale;
		_vector tempRotation;
		_vector tempPosition;

		XMMatrixDecompose(&tempScale, &tempRotation, &tempPosition, XMLoadFloat4x4(&m_OldMatrix));
		XMStoreFloat3(&vSourScale, tempScale);
		XMStoreFloat4(&vSourRotation, tempRotation);
		XMStoreFloat3(&vSourPosition, tempPosition);

		vDestScale = m_KeyFrames.front().vScale;
		vDestRotation = m_KeyFrames.front().vRotation;
		vDestPosition = m_KeyFrames.front().vPosition;

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));

	}
	else
	{
		vScale = m_KeyFrames.front().vScale;
		vRotation = m_KeyFrames.front().vRotation;
		vPosition = m_KeyFrames.front().vPosition;
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));

	if (nullptr != pNode)
	{
		pNode->SetTransformation(TransformationMatrix);
	}

	return iCurrentKeyFrame;
}

void CChannel::SetOldMatrix(_fmatrix OldMatrix)
{
	XMStoreFloat4x4(&m_OldMatrix, OldMatrix);
}

_uint CChannel::GetNumKeyFrame()
{
	return (_uint)m_KeyFrames.size();
}

CChannel * CChannel::Create(aiNodeAnim * pAIChannel)
{
	CChannel*			pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{

}

