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

public:
	const GAME_PLAY RecvGameStart();

	void SendPlayerTransform(C2S_PACKET_PLAYER_TRANSFORM tNewTransform);
	void SendPlayerTransform(const FVector& vPlayerPosition, const FRotator& vPlayerRotation, const float& fVelocityZ, const float& fSpeed);

	bool RecvPlayerTransform(S2C_PACKET_PLAYER_TRANSFORM& tOutPacket);
	
	void SendPlayerInputData(C2S_PACKET_PLAYER_INPUT& tBakuInputData);
	bool RecvPlayerInputData(S2C_PACKET_PLAYER_INPUT& tOutPacket);

public:
	void SetSyncTime(bool bSyncTime) { m_bSyncTime = bSyncTime; }
	const bool& GetSyncTime() { return m_bSyncTime; }

private:
	char* SERVERIP = (char*)"222.117.52.76";
	bool m_bSyncTime = false;
	SOCKETINFO m_tClientSock;
};



