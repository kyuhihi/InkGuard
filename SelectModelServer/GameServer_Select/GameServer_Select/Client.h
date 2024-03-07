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
		
		int recvbytes;

		int totalSendLen;// 총 보내야하는 길이
		int sendbytes;
	};

public:
	CClient(SOCKET sock);
	~CClient();

public://일반함수
	void Initialize(SOCKET sock);
	void Release();
	
public://Getter Setter
	const SOCKETINFO* GetSocketInfo() { return &m_tSockInfo; }
	const S2C_PACKET_PLAYER_TRANSFORM& GetOtherPlayerTransform() { return m_pPlayer->GetTransform(); }

	void SetOtherClient(CClient* pOtherClient) { m_pOtherClient = pOtherClient; }

public:		//Packet
	bool RecvPacket();
	bool SendPacket();

private:	//Packet
	void ConductPacket(const CPacket& Packet);

private:
	SOCKETINFO m_tSockInfo;
	CLIENT_STATE m_eState = STATE_END;
	CPlayer* m_pPlayer = nullptr;

	CClient* m_pOtherClient = nullptr;
};

