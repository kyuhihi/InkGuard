#pragma once
#include "Include.h"
#include "Packet.h"
#include "Player.h"

class CClient
{
public:
	struct SOCKETINFO
	{
		SOCKET sock;
		bool bMatchMakingSuccess = false;

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

	void SetOtherClient(CClient* pOtherClient);
	void SetClientState(const CLIENT_STATE eNewState) { m_eState = eNewState; }

public:		//Public Packet
	bool RecvPacket();
	bool SendPacket();

private:	//Private Packet
	void ConductPacket(const CPacket& Packet);

private:
	SOCKETINFO m_tSockInfo;
	CLIENT_STATE m_eState = STATE_END;
	CPlayer* m_pPlayer = nullptr;

	CClient* m_pOtherClient = nullptr;
};

