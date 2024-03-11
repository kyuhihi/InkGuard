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
{// Ŭ���̾�Ʈ ���� ���
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(m_tSockInfo.sock, (struct sockaddr*)&clientaddr, &addrlen);

	// Ŭ���̾�Ʈ ���� ���
	char addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		addr, ntohs(clientaddr.sin_port));

	// ���� �ݱ�
	closesocket(m_tSockInfo.sock);

	if(m_pOtherClient)
		m_pOtherClient->SetOtherClient(nullptr);

	delete m_pPlayer;
	m_pPlayer = nullptr;

	
}

void CClient::PutInReadOrWriteSet(const fd_set& ReadSet, const fd_set& WriteSet)
{
	if ((m_eState == STATE_READY) || (m_tSockInfo.totalSendLen != 0)) { // �ٸ� Ŭ���̾�Ʈ�� ���ٸ� ready�ؾ��ؼ� readSet�� �־����.
		FD_SET(m_tSockInfo.sock, &WriteSet);	// �ش� ���Ͽ� ���� �����͸� �������ϴ� Ÿ�̹��̴�?-> WriteSet�� �߰�
	}
	else {
		FD_SET(m_tSockInfo.sock, &ReadSet);		// �ش� ���Ͽ� ���� �����͸� �޾ƾ��ϴ� Ÿ�̹��̴�?-> ReadSet�� �߰�
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
{// ������ ������
	if (!m_tSockInfo.bMatchMakingSuccess)
		return true;

	if (m_tSockInfo.totalSendLen == m_tSockInfo.sendbytes) {// �̰� 
		err_display("total != sendbytes");//�̰� ���߿� ���� �պ�.
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
		if ((pPacket->bRecvTransform) && (m_tSockInfo.sendbytes == 0))// �޾ƾ��ϴ� Ÿ�̹��̰� ���������� �ʴ´ٸ�.
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