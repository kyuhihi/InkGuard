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
	if ((m_tSockInfo.totalSendLen != 0)) { // 다른 클라이언트가 없다면 ready해야해서 readSet에 넣어야함.
		FD_SET(m_tSockInfo.sock, &WriteSet);	// 해당 소켓에 대해 데이터를 보내야하는 타이밍이다?-> WriteSet에 추가
	}
	else {
		if (m_eState == STATE_READY && IsInitializedSoldierMgr())
		{
			FD_SET(m_tSockInfo.sock, &WriteSet);
			return;
		}
		FD_SET(m_tSockInfo.sock, &ReadSet);		// 해당 소켓에 대해 데이터를 받아야하는 타이밍이다?-> ReadSet에 추가
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
	else{ //통신 끊긴것. 이건 나중에 바꿔야함.. 좀더 고민쓰.
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

void CClient::ConductPacket(const CPacket& Packet) //받은 패킷을 set하고, 보낼 패킷의 구조체를 저장해둔다.
{
	switch (m_eState)
	{
	case STATE_READY:
	{
		C2S_PACKET_GAMESTART* pPacket = reinterpret_cast<C2S_PACKET_GAMESTART*>(Packet.m_pBuf);
		m_pSoldierMgr->Initialize(*pPacket);// 내 솔져들 객체만 생성. 다음에 언제 보내주냐?
		if ((m_pOtherClient != nullptr) && (m_pOtherClient->IsInitializedSoldierMgr()))
		{//여기서 게임스타트 패킷 보내자.
			SendGameStartPacket();
			m_pOtherClient->SendGameStartPacket();
		}

		break;
	}
	case STATE_TRANSFORM: 
	{
		C2S_PACKET_PLAYER_TRANSFORM* pPacket = reinterpret_cast<C2S_PACKET_PLAYER_TRANSFORM*>(Packet.m_pBuf);
		m_pPlayer->SetTransform(*pPacket);

		if ((pPacket->bRecvTransform) && (m_tSockInfo.sendbytes == 0))// 받아야하는 타이밍이고 보내고있지 않는다면.
		{
			S2C_PACKET_PLAYER_TRANSFORM tSendPacket = m_pOtherClient->GetOtherPlayerTransform();
			//tSendPacket.cGamePlay = m_tSockInfo.eGamePlayTeam;// 게임플레이는 본인걸로 채운다.
			m_tSockInfo.totalSendLen = sizeof(tSendPacket);
			m_tSockInfo.cBuf = new char[m_tSockInfo.totalSendLen];
			memcpy(m_tSockInfo.cBuf, &tSendPacket, m_tSockInfo.totalSendLen);
		}
		else
		{// 보낼 패킷 타이밍이 아니라면 바로 스테이트를 변경해도됨.
			SetClientState(STATE_INPUT);
		}
		break;
	}
	case STATE_INPUT: 
	{
		C2S_PACKET_PLAYER_INPUT* pPacket = reinterpret_cast<C2S_PACKET_PLAYER_INPUT*>(Packet.m_pBuf);
		m_pPlayer->SetInputs(*pPacket);

		if ((m_tSockInfo.sendbytes == 0))// 인풋 패킷은 매 프레임 마다 보내도록한다.
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
		SetClientState(STATE_TRANSFORM); //다시 트랜스폼으로 돌리는걸로 일단 설정 패킷 추가하면 바꿀것.
		break;
	case STATE_END:
		break;
	default:
		break;
	}

	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(m_tSockInfo.sock, (struct sockaddr*)&clientaddr, &addrlen);

	//cout << ntohs(clientaddr.sin_port)<< strState << " 다 보냈어." << endl;

}

#pragma endregion Packet