#pragma once
#include "Include.h"
#include "Packet.h"
#include "Player.h"
#include "SoldierMgr.h"


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

public:
	CClient(SOCKET sock);
	~CClient();

public://일반함수
	void Initialize(SOCKET sock);
	void Release();

	void PutInReadOrWriteSet(const fd_set& ReadSet, const fd_set& WriteSet);

public:	//Getter Setter
	const SOCKETINFO* GetSocketInfo() { return &m_tSockInfo; }
	const S2C_PACKET_PLAYER_TRANSFORM GetOtherPlayerTransform() { return m_pPlayer->GetTransform(); }
	const S2C_PACKET_PLAYER_INPUT GetOtherPlayerInputs() { return m_pPlayer->GetInputs(); }

	void SetTeam(const GAME_PLAY& eTeam) { m_tSockInfo.eGamePlayTeam = eTeam; }
	void SetOtherClient(CClient* pOtherClient);
	void SetClientState(const CLIENT_STATE eNewState) { m_eState = eNewState; }
	

public:		//Public Packet
	bool RecvPacket();
	bool SendPacket();

private:	//Private Packet
	void ConductPacket(const CPacket& Packet);
	void SendComplete();

private:
	SOCKETINFO m_tSockInfo;
	CLIENT_STATE m_eState = STATE_END;
	CPlayer* m_pPlayer = nullptr;

	CClient* m_pOtherClient = nullptr;
	SoldierMgr* m_pSoldierMgr = nullptr;
};

