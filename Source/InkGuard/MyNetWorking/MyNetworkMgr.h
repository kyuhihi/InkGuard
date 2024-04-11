#pragma once

#include "../InkGuard.h"

class MyNetworkMgr
{
#pragma region Declare Singleton
public:
	static MyNetworkMgr* GetInstance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new MyNetworkMgr;	//인스턴스가 없다면 하나 생성합니다.

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

private:// 싱글톤 클래스는 생성자를 private로 관리합니다.
	MyNetworkMgr();
	~MyNetworkMgr();

private:
	static MyNetworkMgr* m_pInstance;

#pragma endregion
private:
	void Initialize();
	void Tidy();
	
	void InitializeSoldierInfo();

public:
	//void OpenMainGame();
	void SendGameStart();
	void RecvGameStart();

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

private:
	char* SERVERIP = (char*)"127.0.0.1";
	bool m_bSyncTime = false;
	SOCKETINFO m_tClientSock;

	GAME_PLAY m_eGameTeam  = GAME_PLAY::GAME_END;;
	bool m_bGameStart = false;
	SOLDIERINFO m_tSoldierInfo[SOLDIER_MAX_CNT]; //내꺼 솔져 정보.
};



