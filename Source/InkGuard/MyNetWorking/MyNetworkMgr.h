#pragma once

#include "../InkGuard.h"

class MyNetworkMgr
{
#pragma region Declare Singleton
public:
	static MyNetworkMgr* GetInstance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new MyNetworkMgr;	//�ν��Ͻ��� ���ٸ� �ϳ� �����մϴ�.

		return m_pInstance;
	}
	static void DestroyInstance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:// �̱��� Ŭ������ �����ڸ� private�� �����մϴ�.
	MyNetworkMgr();
	~MyNetworkMgr();

private:
	static MyNetworkMgr* m_pInstance;

#pragma endregion
private:
	void Initialize();
	void Tidy();

	static DWORD WINAPI RecvGameStart(LPVOID arg);
public:
	void OpenMainGame();

	void SendPlayerTransform(C2S_PACKET_PLAYER_TRANSFORM tNewTransform);
	void SendPlayerTransform(const FVector& vPlayerPosition, const FRotator& vPlayerRotation, const float& fVelocityZ, const float& fSpeed);

	bool RecvPlayerTransform(S2C_PACKET_PLAYER_TRANSFORM& tOutPacket);
	
	void SendPlayerInputData(C2S_PACKET_PLAYER_INPUT& tBakuInputData);
	bool RecvPlayerInputData(S2C_PACKET_PLAYER_INPUT& tOutPacket);

public:
	const bool& GetGameStart() { return m_bGameStart; }

	void SetSyncTime(bool bSyncTime) { m_bSyncTime = bSyncTime; }
	const bool& GetSyncTime() { return m_bSyncTime; }

	void SetSoldierInfo(int iIndex, int iSoldierType, int iTargetTerritory);

	void SetGameStartMutexLock() { m_GameStartMutex.lock(); };
	void SetGameStartMutexUnLock() { m_GameStartMutex.unlock(); };

private:
	char* SERVERIP = (char*)"127.0.0.1";
	bool m_bSyncTime = false;
	SOCKETINFO m_tClientSock;

	static mutex m_GameStartMutex;
	static GAME_PLAY m_eGameTeam;
	static bool m_bGameStart;
	static SOLDIERINFO m_tSoldierInfo[SOLDIER_MAX_CNT]; //���� ���� ����.

	HANDLE m_hThread;
};



