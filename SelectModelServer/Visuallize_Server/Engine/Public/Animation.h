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
	//���� �ִϸ��̼��� ���° Ű���������� ǥ���� ���� �Լ�
	_uint GetCurrentKeyFrame();
	

	//���° Ű������, �Լ� ������ ��, �Լ� �̸�
	void SetKeyFrameEvent(_uint iKeyFrame, const char* EventName);

private:

	_uint						m_iCurrentKeyFrame = 0;
	/* �� �ִϸ��̼��� �����ϱ����� ���Ǵ� ���� ����. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

	/* �ִϸ��̼� ����ϴµ� �ɸ��� ��ü�ð�. */
	_float						m_fDuration = 0.f;

	/* �ִϸ��̼��� �ʴ� ��� �ӵ�. */
	_float						m_fTickPerSecond = 0.f;

	_float						m_fPlayTime = 0.f;

	//�ִϸ��̼��� ��������
	_bool						m_bBlending = false;

	/* �ִϸ��̼ǰ� �ִϸ��̼� ������ ���� �ð� */
	_float						m_fBlendTime = 0.25f;

	/* �ִϸ��̼��� �Ӽ���. �ִϸ��̼� ���� �� ó������ �ǵ������� �ƴҰ��� */
	_bool						m_bLoop = true;

	/* �ִϸ��̼��� �̸� */
	char						m_szName[MAX_PATH] = "";

private: /* ������ �ִϸ��̼� ���� ���� ������. */
	vector<class CHierarchyNode*>	m_HierarchyNodes;
	vector<KEYFRAMEINFO>			m_ChannelKeyFrames;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation);
	CAnimation* Clone(class CModel* pModel);
	virtual void Free() override;
};

END