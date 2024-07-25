#pragma once

#include "../InkGuard.h"
#include "../Customs/CustomStructs.h"

class MyNetworkMgr
{
#pragma region Declare
#pragma region Singleton
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
#pragma region Struct
	struct AdditionalPacket {
		bool bUse = false;
		int iDataSize = 0;
		EAdditionalPacketType ePacketType;
		char pData[sizeof(C2S_PACKET_ADDITIONAL_FLOAT3x3)]; //���߿� ��Ŀ���� �ٲܰ�.
	};

#pragma endregion
#pragma endregion

private:
	void Initialize();
	void InitializeSocket();
	void InitializeAdditionalList();

	void Tidy();
	
public:
	void SendGameStart();
	void RecvGameStart();
	
	void RegisterSpawnMgr(class ASpawnMgr* pSpawnMgr) { m_pSpawnMgr = pSpawnMgr; }
	void SendPlayerTransform(const FVector& vPlayerPosition, const FRotator& vPlayerRotation, const float& fVelocityZ, const float& fSpeed, const float& fBakuHP);
	bool RecvPlayerTransform(S2C_PACKET_PLAYER_TRANSFORM& tOutPacket);// ������ �����ε� ��������..
	
	void SendPlayerInputData(C2S_PACKET_PLAYER_INPUT& tBakuInputData);
	bool RecvPlayerInputData(S2C_PACKET_PLAYER_INPUT& tOutPacket);

	void AppendDataToAdditionalList(bool bSendVec, EAdditionalPacketType eNewType, const C2S_PACKET_ADDITIONAL_FLOAT3x3 tNewPacket);
	int SendAdditionalData();
	void RecvAdditionalData();
	void ConductAdditionalData(const char* pNewPacket);
	void FindAdditionalData(EAdditionalPacketType eFindType, C2S_PACKET_ADDITIONAL_FLOAT3x3& tOutData);

	void ClearAdditionalPacket();

public:
	const bool& GetGameStart() { return m_bGameStart; }
	const GAME_PLAY& GetTeamColor() { return m_eGameTeam; }

	void SetSyncTime(bool bSyncTime) { m_bSyncTime = bSyncTime; }
	const bool& GetSyncTime() { return m_bSyncTime; }

	const float GetServerGameTime() const { return m_fServerTime; }

	void SetSoldierInfo(int iIndex, int iSoldierType, int iTargetTerritory);

	void SetReservedOpenLevel(bool bNewValue);
	const bool& GetReservedOpenLevel() { return m_bReservedOpenLevel; };

	const short& GetAdditionalSendPacketSize() { return m_sSendAdditionalPacketSize; }

private:
	bool RequestRemainVectorIndex(bool bSendVec, int& iOutVectorIndex);
	void ChangeSendSoldierTransformCnt(int& iChangeCnt);

public:
	static SOLDIERINFO m_tSoldierInfo[SOLDIER_MAX_CNT];
	static SOLDIERINFO m_tOtherSoldierInfo[SOLDIER_MAX_CNT];
	static string SERVERIP; /*= (char*)"192.168.0.104";*/

private:
	bool m_bSyncTime = false;
	SOCKETINFO m_tClientSock;

	GAME_PLAY m_eGameTeam  = GAME_PLAY::GAME_END;		//�� ����
	bool m_bGameStart = false;
	bool m_bReservedOpenLevel = false;					// ���ӽ�ŸƮ ��Ŷ�� �ް� ���·��� �ϱ� �������� true�Ұ���.
	
	class ASpawnMgr* m_pSpawnMgr = nullptr;

	vector<AdditionalPacket> m_SendAdditionalPacketVec;
	short m_sSendAdditionalPacketSize = 0;

	vector<AdditionalPacket> m_RecvAdditionalPacketVec;
	short m_sRecvAdditionalPacketSize = 0;

	int m_iSendSoldierCnt = 4;
	int m_iRecvSoldierCnt = 4;

	float m_fServerTime = TOTAL_GAME_TIME;

	list<string> m_DebugStringTable;
	void MakeDebugStringTable(const char* pString);
};



