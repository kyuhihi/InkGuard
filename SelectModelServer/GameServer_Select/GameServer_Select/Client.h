#pragma once
#include "Include.h"
#include "Packet.h"
#include "SoldierMgr.h"
#include "Player.h"

class CClient
{
public:
	struct SOCKETINFO
	{
		SOCKET sock;
		bool bMatchMakingSuccess = false;
		GAME_PLAY eGamePlayTeam = GAME_END;// ���� �������� �������

		int totalSendLen;// �� �������ϴ� ����
		int sendbytes;
		char* cBuf = nullptr;
	};

	enum ADDITIONAL_STATE_CONDITION { CONDITION_RECV, CONDITION_SEND, CONDITION_END };
	enum FRAME_SET { FRAME_WRITE, FRAME_READ, FRAME_END };

public:
	CClient(SOCKET sock);
	~CClient();

public://�Ϲ��Լ�
	void Initialize(SOCKET sock);
	void Release();

	bool PutInReadOrWriteSet(const fd_set& ReadSet, const fd_set& WriteSet); // return ���� false��� �ٽ� ȣ���Ұ�.

public:	//Getter Setter
	bool IsInitializedSoldierMgr();
	const SOCKETINFO* GetSocketInfo() { return &m_tSockInfo; }

	void GetGameStartPacket(S2C_PACKET_GAMESTART& tOutGameStartPacket);
	const S2C_PACKET_PLAYER_TRANSFORM GetOtherPlayerTransform() { return m_pPlayer->GetTransform(); }
	void GetOtherSoldiersTransform(C2S_PACKET_SOLDIER_TRANSFORM* pSendBuffer) { return m_pSoldierMgr->GetSoldiersPacket(pSendBuffer); }
	const S2C_PACKET_PLAYER_INPUT GetOtherPlayerInputs() { return m_pPlayer->GetInputs(); }

	void CalculateSendAdditionalPacket(char*& pOtherClientSendBuf, int& iOtherSendBufferSize) { m_pPlayer->CalculateSendAdditionalPacket(pOtherClientSendBuf, iOtherSendBufferSize); };
	const int& GetRemAdditionalSize() const { return m_pPlayer->GetRemAdditionalSize(); };
	void ClearPlayerUsedData() { m_pPlayer->ClearUsedData(); };

	void SetTeam(const GAME_PLAY& eTeam) { m_tSockInfo.eGamePlayTeam = eTeam; }
	void SetOtherClient(CClient* pOtherClient);
	void SetClientState(const CLIENT_STATE eNewState) { m_eState = eNewState; }

	void MakeDebugStringtable(const char* FuncName);
	
private:
	void ReserveAdditionalState();
	void ClearSendBuffer();
	void ChangeRecvSoldierTransformCnt();
	void ConductTransformPacket(bool& bSendTransformDuty, const CPacket& Packet);

public:		//Public Packet
	bool RecvPacket();
	//bool RecvRemainPacket(char*& pBuf, );
	bool SendPacket();

	void SendGameStartPacket();

private:	//Private Packet
	void ConductPacket(const CPacket& Packet);
	void SendComplete(bool doDebugString = true);

private:
	SOCKETINFO m_tSockInfo;
	CLIENT_STATE m_eState = STATE_END;

	CLIENT_STATE m_eReservedState = STATE_END;
	CPlayer* m_pPlayer = nullptr;

	CClient* m_pOtherClient = nullptr;
	CSoldierMgr* m_pSoldierMgr = nullptr; // ������ ���Ե� ��ī�� ���Ǿ���� �����ϴ� ����ü��. ���� 9������ �����Ұ���.

	bool m_bReserved_Additional_State[CONDITION_END] = { false,false };

	list<string>m_StringTable; //����׿�.

	FRAME_SET m_eCurSet = FRAME_SET::FRAME_END;


	int m_iRecvSoldiersCnt = 4;
};

