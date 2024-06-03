#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
	using KEYFRAMEINFO = struct 
	{
		_uint iKeyFrame;
		vector<string> EventName;
	};

	typedef void(*FnPtr)(void);

private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT InitializePrototype(aiAnimation* pAIAnimation);
	HRESULT Initialize(class CModel* pModel);
	_bool PlayAnimation(_float fTimeDelta);
	_bool PlayAnimation(_float fTimeDelta, vector<string>* EventName);

	void ChangeAnimation(CAnimation* pPrevAnimation);
	void ResetKeyFrames();
	_bool IsBlending() {
		return m_bBlending;
	}
	void SetBlendTimeAndLoop(_float fBlendTime, _bool bLoop) {
		m_fBlendTime = fBlendTime;
		m_bLoop = bLoop;
	}

	class CChannel* GetChannel(_uint iChannelIndex);

	/* For. AnimationTool */
	_uint GetNumKeyFrame();
	const char* GetName()
	{
		return m_szName;
	}
	//현재 애니메이션의 몇번째 키프레임인지 표현을 위한 함수
	_uint GetCurrentKeyFrame();
	

	//몇번째 키프레임, 함수 포인터 맵, 함수 이름
	void SetKeyFrameEvent(_uint iKeyFrame, const char* EventName);

private:

	_uint						m_iCurrentKeyFrame = 0;
	/* 이 애니메이션을 구동하기위해 사용되는 뼈의 갯수. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

	/* 애니메이션 재생하는데 걸리는 전체시간. */
	_float						m_fDuration = 0.f;

	/* 애니메이션의 초당 재생 속도. */
	_float						m_fTickPerSecond = 0.f;

	_float						m_fPlayTime = 0.f;

	//애니메이션의 보간여부
	_bool						m_bBlending = false;

	/* 애니메이션과 애니메이션 사이의 보간 시간 */
	_float						m_fBlendTime = 0.25f;

	/* 애니메이션의 속성값. 애니메이션 종료 후 처음으로 되돌릴건지 아닐건지 */
	_bool						m_bLoop = true;

	/* 애니메이션의 이름 */
	char						m_szName[MAX_PATH] = "";

private: /* 복제된 애니메이션 마다 따로 가진다. */
	vector<class CHierarchyNode*>	m_HierarchyNodes;
	vector<KEYFRAMEINFO>			m_ChannelKeyFrames;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation);
	CAnimation* Clone(class CModel* pModel);
	virtual void Free() override;
};

END