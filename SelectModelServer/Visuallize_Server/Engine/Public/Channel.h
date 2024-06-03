#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	const char* GetName() const {
		return m_szName;
	}

public:
	HRESULT Initialize(aiNodeAnim*	pAIChannel);
	_uint UpdateTransformation(_float fPlayTime, _uint iCurrentKeyFrame, class CHierarchyNode* pNode);
	_uint UpdateTransformation(_float fPlayTime, _uint iCurrentKeyFrame, class CHierarchyNode* pNode, _uint& iAnimKeyFrame);

	_uint BlendingTransformation(_float fPlayTime, _uint iCurrentKeyFrame, class CHierarchyNode* pNode, _float fBlendTime);
	void SetOldMatrix(_fmatrix OldMatrix);

	_matrix GetOldMatrix() 
	{
		return XMLoadFloat4x4(&m_OldMatrix);
	}

	void ResetOldMatrix()
	{
		XMStoreFloat4x4(&m_OldMatrix, XMMatrixIdentity());
	};

	/* For. AnimationTool */
	_uint GetNumKeyFrame();

	_uint GetCurrentKeyFrame() {
		return m_iCurrentKeyFrame;
	};
private:
	char							m_szName[MAX_PATH] = "";

	_uint							m_iNumKeyFrames = 0;
	vector<KEYFRAME>				m_KeyFrames;	

	_float4x4						m_OldMatrix;
	_uint							m_iCurrentKeyFrame = 0;
public:
	static CChannel* Create(aiNodeAnim*	pAIChannel);
	virtual void Free() override;
};

END