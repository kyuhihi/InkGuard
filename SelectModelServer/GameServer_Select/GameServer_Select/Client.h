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
		GAME_PLAY eGamePlayTeam = GAME_END;// 팀이 레드인지 블루인지

		int totalSendLen;// 총 보내야하는 길이
		int sendbytes;
		char* cBuf = nullptr;
	};

	enum ADDITIONAL_STATE_CONDITION { CONDITION_RECV, CONDITION_SEND, CONDITION_END };
	enum FRAME_SET { FRAME_WRITE, FRAME_READ, FRAME_END };

public:
	CClient(SOCKET sock);
	~CClient();

public://일반함수
	void Initialize(SOCKET sock);
	void Release();

	bool PutInReadOrWriteSet(const fd_set& ReadSet, const fd_set& WriteSet); // return 값이 false라면 다시 호출할것.

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
	CSoldierMgr* m_pSoldierMgr = nullptr; // 솔져로 포함된 다카와 스피어맨을 관리하는 집약체임. 내팀 9마리만 관리할거임.

	bool m_bReserved_Additional_State[CONDITION_END] = { false,false };

	list<string>m_StringTable; //디버그용.

	FRAME_SET m_eCurSet = FRAME_SET::FRAME_END;


	int m_iRecvSoldiersCnt = 4;
};

