#include "SoundManager.h"
//#include "Engine_Struct.h"

IMPLEMENT_SINGLETON(CSoundManager)

CSoundManager::CSoundManager() :
	m_pSystem(nullptr)
{

}

HRESULT CSoundManager::Initialize()
{
	if (FMOD_OK != FMOD::System_Create(&m_pSystem))
		return E_FAIL;
	
	if (FMOD_OK != m_pSystem->init(32, FMOD_INIT_NORMAL, NULL))
		return E_FAIL;

	if (FAILED(LoadSoundFile()))
		return E_FAIL;

	return S_OK;
}

void CSoundManager::PlayEffect(_tchar* pSoundKey, float fVolume)
{
	SOUNDMAP::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[SOUND_CHANNEL_EFFECT]);

	m_pChannelArr[SOUND_CHANNEL_EFFECT]->setVolume(fVolume);

	m_pSystem->update();
}

void CSoundManager::PlayLoopEffect(_tchar* pSoundKey, float fVolume)
{
	SOUNDMAP::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[SOUND_CHANNEL_LOOPEFFECT]);

	m_pChannelArr[SOUND_CHANNEL_LOOPEFFECT]->setMode(FMOD_LOOP_NORMAL);

	m_pChannelArr[SOUND_CHANNEL_LOOPEFFECT]->setVolume(fVolume);

	m_pSystem->update();
}

void CSoundManager::PlayOneShotEffect(_tchar* pSoundKey, float fVolume)
{
	SOUNDMAP::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	bool bPlay = FALSE;

	m_pChannelArr[SOUND_CHANNEL_ONEEFFECT]->isPlaying(&bPlay);
	
	if (!bPlay)
	{
		m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[SOUND_CHANNEL_ONEEFFECT]);
	}

	m_pChannelArr[SOUND_CHANNEL_ONEEFFECT]->setVolume(fVolume);

	m_pSystem->update();
}


void CSoundManager::PlaySoundChannel(_tchar* pSoundKey, SOUND_CHANNEL eID, float fVolume)
{
	SOUNDMAP::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), 
		[&](auto& iter)->bool 
	{
		return !lstrcmp(pSoundKey, iter.first);
	});
	
	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE; 

	m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[eID]);

	m_pChannelArr[eID]->setVolume(fVolume);

	m_pSystem->update();
}

void CSoundManager::PlayBGM(_tchar* pSoundKey, float fVolume)
{
	SOUNDMAP::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});
	
	if (iter == m_mapSound.end())
		return;

	_bool bPlay = false;

	if (m_pChannelArr[SOUND_CHANNEL_BGM]->isPlaying(&bPlay)) 
	{
		m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[SOUND_CHANNEL_BGM]);
	}

	m_pChannelArr[SOUND_CHANNEL_BGM]->setMode(FMOD_LOOP_NORMAL);
	
	m_pChannelArr[SOUND_CHANNEL_BGM]->setVolume(fVolume);

	m_pSystem->update();
}


void CSoundManager::StopSound(SOUND_CHANNEL eID)
{
	m_pChannelArr[eID]->stop();
}

void CSoundManager::StopEffect()
{
	StopSound(SOUND_CHANNEL_EFFECT);
	StopSound(SOUND_CHANNEL_ONEEFFECT);

}

void CSoundManager::StopLoopEffect()
{
	StopSound(SOUND_CHANNEL_LOOPEFFECT);
}

void CSoundManager::StopBGM() 
{
	StopSound(SOUND_CHANNEL_BGM);
}

void CSoundManager::StopAll()
{
	for (int i = 0; i < SOUND_CHANNEL_END; ++i)
		m_pChannelArr[i]->stop();
}

void CSoundManager::SetChannelVolume(SOUND_CHANNEL eID, float fVolume)
{
	m_pChannelArr[eID]->setVolume(fVolume);
	m_pSystem->update();
}

HRESULT CSoundManager::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd; 

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst("../Bin/Resources/Sound/*.*", &fd);

	if (handle == -1)
		return E_FAIL; 

	int iResult = 0; 

	char szCurPath[128] = "../Bin/Resources/Sound/";
	char szFullPath[128] = ""; 

	while (iResult != -1) 
	{
		if (fd.attrib & _A_SUBDIR) {
			iResult = _findnext(handle, &fd);
			continue;
		}

		strcpy_s(szFullPath, szCurPath); 

		strcat_s(szFullPath, fd.name);

		FMOD::Sound* pSound = nullptr;

		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, NULL, 0, &pSound);
		// FMOD_HARDWARE

		if (eRes == FMOD_OK)
		{
			_uint iLength = static_cast<_uint>(strlen(fd.name) + 1); 

			_tchar* pSoundKey = new _tchar[iLength];
			ZeroMemory(pSoundKey, sizeof(_tchar) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}

		else {
			return E_FAIL;
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	m_pSystem->update();

	_findclose(handle);
	
	return S_OK;
}

void CSoundManager::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		Mypair.second->release();
	}

	m_mapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
}
