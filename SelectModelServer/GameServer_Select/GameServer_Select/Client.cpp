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

	m_tSockInfo.totalSendLen = m_tSockInfo.sendbytes = 0;
	
	SetClientState(STATE_READY);
	
	m_pPlayer = new CPlayer;
	m_pSoldierMgr = new CSoldierMgr;
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

	if (m_pOtherClient) {
		m_pOtherClient->SetOtherClient(nullptr);
	}
	delete m_pPlayer;
	m_pPlayer = nullptr;
	
	delete m_pSoldierMgr;
	m_pSoldierMgr = nullptr;
}

void CClient::PutInReadOrWriteSet(const fd_set& ReadSet, const fd_set& WriteSet)
{
	if ((m_tSockInfo.totalSendLen != 0)) { // �ٸ� Ŭ���̾�Ʈ�� ���ٸ� ready�ؾ��ؼ� readSet�� �־����.
		FD_SET(m_tSockInfo.sock, &WriteSet);	// �ش� ���Ͽ� ���� �����͸� �������ϴ� Ÿ�̹��̴�?-> WriteSet�� �߰�
	}
	else {
		if (m_eState == STATE_READY && IsInitializedSoldierMgr())
		{
			FD_SET(m_tSockInfo.sock, &WriteSet);
			return;
		}
		FD_SET(m_tSockInfo.sock, &ReadSet);		// �ش� ���Ͽ� ���� �����͸� �޾ƾ��ϴ� Ÿ�̹��̴�?-> ReadSet�� �߰�
	}
}

bool CClient::IsInitializedSoldierMgr()
{
	if (m_pSoldierMgr->GetSoldierVecSize() == 0)
		return false;
	else
		return true;
}

void CClient::SetOtherClient(CClient* pOtherClient)
{ 
	m_pOtherClient = pOtherClient;
	if (pOtherClient) {
		m_tSockInfo.bMatchMakingSuccess = true;
	}
	else{ //��� �����. �̰� ���߿� �ٲ����.. ���� ��ξ�.
		SetClientState(STATE_READY);
		m_tSockInfo.bMatchMakingSuccess = false;
		
		m_tSockInfo.totalSendLen = m_tSockInfo.sendbytes = 0;
		delete[] m_tSockInfo.cBuf;
		m_tSockInfo.cBuf = nullptr;
		
		m_tSockInfo.eGamePlayTeam = GAME_PLAY::GAME_END;
	}
}

#pragma region Packet

bool CClient::RecvPacket()
{
	CPacket tNewPacket(m_eState);

	int retval = recv(m_tSockInfo.sock, tNewPacket.m_pBuf, tNewPacket.m_iBufferSize, 0);
	
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
			SendComplete();
			
		}
	}


	
	return true;
}

void CClient::SendGameStartPacket()
{	
	S2C_PACKET_GAMESTART tGameStartPacket;
	m_pSoldierMgr->GetGameStartPacket(tGameStartPacket);
	tGameStartPacket.cGamePlay = m_tSockInfo.eGamePlayTeam;

	m_tSockInfo.totalSendLen = sizeof(tGameStartPacket);
	m_tSockInfo.cBuf = new char[m_tSockInfo.totalSendLen];

	memcpy(m_tSockInfo.cBuf, &tGameStartPacket, m_tSockInfo.totalSendLen);
}

void CClient::ConductPacket(const CPacket& Packet) //���� ��Ŷ�� set�ϰ�, ���� ��Ŷ�� ����ü�� �����صд�.
{
	switch (m_eState)
	{
	case STATE_READY:
	{
		C2S_PACKET_GAMESTART* pPacket = reinterpret_cast<C2S_PACKET_GAMESTART*>(Packet.m_pBuf);
		m_pSoldierMgr->Initialize(*pPacket);// �� ������ ��ü�� ����. ������ ���� �����ֳ�?
		if ((m_pOtherClient != nullptr) && (m_pOtherClient->IsInitializedSoldierMgr()))
		{//���⼭ ���ӽ�ŸƮ ��Ŷ ������.
			SendGameStartPacket();
			m_pOtherClient->SendGameStartPacket();
		}

		break;
	}
	case STATE_TRANSFORM: 
	{
		C2S_PACKET_PLAYER_TRANSFORM* pPacket = reinterpret_cast<C2S_PACKET_PLAYER_TRANSFORM*>(Packet.m_pBuf);
		m_pPlayer->SetTransform(*pPacket);

		if ((pPacket->bRecvTransform) && (m_tSockInfo.sendbytes == 0))// �޾ƾ��ϴ� Ÿ�̹��̰� ���������� �ʴ´ٸ�.
		{
			S2C_PACKET_PLAYER_TRANSFORM tSendPacket = m_pOtherClient->GetOtherPlayerTransform();
			//tSendPacket.cGamePlay = m_tSockInfo.eGamePlayTeam;// �����÷��̴� ���ΰɷ� ä���.
			m_tSockInfo.totalSendLen = sizeof(tSendPacket);
			m_tSockInfo.cBuf = new char[m_tSockInfo.totalSendLen];
			memcpy(m_tSockInfo.cBuf, &tSendPacket, m_tSockInfo.totalSendLen);
		}
		else
		{// ���� ��Ŷ Ÿ�̹��� �ƴ϶�� �ٷ� ������Ʈ�� �����ص���.
			SetClientState(STATE_INPUT);
		}
		break;
	}
	case STATE_INPUT: 
	{
		C2S_PACKET_PLAYER_INPUT* pPacket = reinterpret_cast<C2S_PACKET_PLAYER_INPUT*>(Packet.m_pBuf);
		m_pPlayer->SetInputs(*pPacket);

		if ((m_tSockInfo.sendbytes == 0))// ��ǲ ��Ŷ�� �� ������ ���� ���������Ѵ�.
		{
			S2C_PACKET_PLAYER_INPUT tSendPacket = m_pOtherClient->GetOtherPlayerInputs();
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

void CClient::SendComplete()
{
	m_tSockInfo.totalSendLen = m_tSockInfo.sendbytes = 0;
	delete[] m_tSockInfo.cBuf;
	m_tSockInfo.cBuf = nullptr;

	string strState;
	switch (m_eState)
	{
	case STATE_READY:
		SetClientState(STATE_TRANSFORM);
		break;
	case STATE_TRANSFORM:
		strState = " Transform";
		SetClientState(STATE_INPUT);
		break;
	case STATE_INPUT:
		strState = " Input";
		SetClientState(STATE_TRANSFORM); //�ٽ� Ʈ���������� �����°ɷ� �ϴ� ���� ��Ŷ �߰��ϸ� �ٲܰ�.
		break;
	case STATE_END:
		break;
	default:
		break;
	}

	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(m_tSockInfo.sock, (struct sockaddr*)&clientaddr, &addrlen);

	//cout << ntohs(clientaddr.sin_port)<< strState << " �� ���¾�." << endl;

}

#pragma endregion Packet