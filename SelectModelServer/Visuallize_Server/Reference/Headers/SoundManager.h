#pragma once

#include "Base.h"
#include "Engine_Typedef.h"

BEGIN(Engine)

class ENGINE_DLL CSoundManager final : public CBase
{
	DECLARE_SINGLETON(CSoundManager)
private:
	CSoundManager();
	virtual ~CSoundManager() = default;

public:
	HRESULT Initialize();

public:
	void PlaySoundChannel(_tchar* pSoundKey, SOUND_CHANNEL eID, float fVolume);
	void PlayEffect(_tchar* pSoundKey, float fVolume);
	void PlayLoopEffect(_tchar* pSoundKey, float fVolume);
	void PlayOneShotEffect(_tchar* pSoundKey, float fVolume);
	void PlayBGM(_tchar* pSoundKey, float fVolume);
	
	void StopSound(SOUND_CHANNEL eID);
	void StopEffect();
	void StopLoopEffect();
	void StopBGM();
	void StopAll();

	void SetChannelVolume(SOUND_CHANNEL eID, float fVolume);

private:
	HRESULT LoadSoundFile(); 

private:
	// 사운드 리소스 정보를 갖는 객체 
	using SOUNDMAP = map<_tchar*, FMOD::Sound*>;

	SOUNDMAP m_mapSound;
	
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel* m_pChannelArr[SOUND_CHANNEL_END];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD::System* m_pSystem; 

public:
	virtual void Free() override;
};

END