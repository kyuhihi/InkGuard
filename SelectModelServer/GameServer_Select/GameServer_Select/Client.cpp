#include "Client.h"

CClient::CClient(SOCKET sock)
{
	Initialize(sock);
}

CClient::~CClient()
{
	Release();
}

void CClient::Initialize(SOCKET sock)
{
	m_tSockInfo.sock = sock;

	m_tSockInfo.sendbytes = 0;
	m_tSockInfo.totalSendLen = m_tSockInfo.sendbytes;
	
	SetClientState(STATE_READY);
	
	m_pPlayer = new CPlayer;
}

void CClient::Release()
{// 클라이언트 정보 얻기
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(m_tSockInfo.sock, (struct sockaddr*)&clientaddr, &addrlen);

	// 클라이언트 정보 출력
	char addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		addr, ntohs(clientaddr.sin_port));

	// 소켓 닫기
	closesocket(m_tSockInfo.sock);

	if(m_pOtherClient)
		m_pOtherClient->SetOtherClient(nullptr);

	delete m_pPlayer;
	m_pPlayer = nullptr;

	
}

void CClient::PutInReadOrWriteSet(const fd_set& ReadSet, const fd_set& WriteSet)
{
	if ((m_eState == STATE_READY) || (m_tSockInfo.totalSendLen != 0)) { // 다른 클라이언트가 없다면 ready해야해서 readSet에 넣어야함.
		FD_SET(m_tSockInfo.sock, &WriteSet);	// 해당 소켓에 대해 데이터를 보내야하는 타이밍이다?-> WriteSet에 추가
	}
	else {
		FD_SET(m_tSockInfo.sock, &ReadSet);		// 해당 소켓에 대해 데이터를 받아야하는 타이밍이다?-> ReadSet에 추가
	}
}

void CClient::SetOtherClient(CClient* pOtherClient)
{ 
	m_pOtherClient = pOtherClient;
	if (pOtherClient) {
		S2C_PACKET_GAMESTART tGameStartPacket;
		tGameStartPacket.bStart = true;

		m_tSockInfo.totalSendLen = sizeof(tGameStartPacket);
		m_tSockInfo.cBuf = new char[m_tSockInfo.totalSendLen];


		memcpy(m_tSockInfo.cBuf, &tGameStartPacket, m_tSockInfo.totalSendLen);
		m_tSockInfo.bMatchMakingSuccess = true;

	}
	else
	{
		SetClientState(STATE_READY);
		m_tSockInfo.bMatchMakingSuccess = false;
	}
}

#pragma region Packet

bool CClient::RecvPacket()
{
	CPacket tNewPacket(m_eState);
	C2S_PACKET_PLAYER_TRANSFORM tTestPacket;

	int retval = recv(m_tSockInfo.sock, tNewPacket.m_pBuf, tNewPacket.m_iBufferSize, 0);
	//int retval = recv(m_tSockInfo.sock, (char*)&tTestPacket, sizeof(tTestPacket), 0);
	
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (retval == 0) {
		return false;
	}

	ConductPacket(tNewPacket);

	return true;
}

bool CClient::SendPacket()
{// 데이터 보내기
	if (!m_tSockInfo.bMatchMakingSuccess)
		return true;

	if (m_tSockInfo.totalSendLen == m_tSockInfo.sendbytes) {// 이건 
		err_display("total != sendbytes");//이건 나중에 조금 손봐.
		return false;
	}

	int retval = 0;
	retval = send(m_tSockInfo.sock, m_tSockInfo.cBuf + m_tSockInfo.sendbytes, 
		m_tSockInfo.totalSendLen - m_tSockInfo.sendbytes, 0);


	if (retval == SOCKET_ERROR) {//SendFail
		err_display("send()");
		return false;
	}
	else {// SendPacket Success
		m_tSockInfo.sendbytes += retval;
		if (m_tSockInfo.totalSendLen == m_tSockInfo.sendbytes) {//SEND ALL COMPLETE
			m_tSockInfo.totalSendLen = m_tSockInfo.sendbytes = 0;

			delete[] m_tSockInfo.cBuf;
			m_tSockInfo.cBuf = nullptr;
			if (m_eState == STATE_READY)
				SetClientState(STATE_TRANSFORM);
		}
	}


	
	return true;
}

void CClient::ConductPacket(const CPacket& Packet)
{
	switch (m_eState)
	{
	case STATE_TRANSFORM: {
		C2S_PACKET_PLAYER_TRANSFORM* pPacket = reinterpret_cast<C2S_PACKET_PLAYER_TRANSFORM*>(Packet.m_pBuf);
		m_pPlayer->SetTransform(*pPacket);
		if ((pPacket->bRecvTransform) && (m_tSockInfo.sendbytes == 0))// 받아야하는 타이밍이고 보내고있지 않는다면.
		{
			S2C_PACKET_PLAYER_TRANSFORM tSendPacket = m_pOtherClient->GetOtherPlayerTransform();
			m_tSockInfo.totalSendLen = sizeof(tSendPacket);
			m_tSockInfo.cBuf = new char[m_tSockInfo.totalSendLen];
			memcpy(m_tSockInfo.cBuf, &tSendPacket, m_tSockInfo.totalSendLen);

		}

		break;
	}
	case STATE_END:
	default:
		break;
	}
}

#pragma endregion Packet