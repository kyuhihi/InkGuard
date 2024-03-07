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
	m_tSockInfo.recvbytes = 0;
	m_tSockInfo.sendbytes = 0;
	m_eState = STATE_TRANSFORM;
	
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

	m_pOtherClient->SetOtherClient(nullptr);

	delete m_pPlayer;
	m_pPlayer = nullptr;

	
}

#pragma region Packet

bool CClient::RecvPacket()
{
	CPacket tNewPacket(m_eState);

	int retval = recv(m_tSockInfo.sock, tNewPacket.m_pBuf, tNewPacket.m_iBufferSize, MSG_WAITALL);
	
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return false;
	}
	else if (retval == 0) {
		return false;
	}
	else {
		ConductPacket(tNewPacket);

	}
	

	return true;
}

bool CClient::SendPacket()
{// ������ ������
	int retval = 0;
	char* buf = nullptr;
	//retval = send(m_tSockInfo.sock, ptr->buf + ptr->sendbytes,
	//	ptr->recvbytes - ptr->sendbytes, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return false;
	}
	else {
		m_tSockInfo.sendbytes += retval;
		//if (ptr->recvbytes == ptr->sendbytes) {
		//	ptr->recvbytes = ptr->sendbytes = 0;// ���ڼ����� recvbytes�� �˻���. ���߿� ��ĥ��.
		//}
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
		if (pPacket->bRecvTransform)
		{
			S2C_PACKET_PLAYER_TRANSFORM tSendPacket = m_pOtherClient->GetOtherPlayerTransform();
			SendPacket();
		}
		break;
	}
	case STATE_END:
	default:
		break;
	}
}

#pragma endregion Packet