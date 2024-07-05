#include "Client.h"

#define MAX_DEBUG_STRING_WRITE 20

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
	
	m_pPlayer = new CPlayer(this);
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

bool CClient::PutInReadOrWriteSet(const fd_set& ReadSet, const fd_set& WriteSet) 
{
	FRAME_SET eCurSet = FRAME_SET::FRAME_END;
	if (m_eState == STATE_ADDITIONAL)
	{
		if (m_bReserved_Additional_State[CONDITION_RECV])
		{
			FD_SET(m_tSockInfo.sock, &ReadSet);// ReadSet에 추가
			eCurSet = FRAME_READ;
		}
		else if (m_bReserved_Additional_State[CONDITION_SEND])
		{
			FD_SET(m_tSockInfo.sock, &WriteSet);// Write에 추가
			eCurSet = FRAME_WRITE;
		}
		else {
			SendComplete(false);  
			return false;
		}
	}
	else {
		if ((m_tSockInfo.totalSendLen != 0)) { // 다른 클라이언트가 없다면 ready해야해서 readSet에 넣어야함.
			FD_SET(m_tSockInfo.sock, &WriteSet);	// 해당 소켓에 대해 데이터를 보내야하는 타이밍이다?-> WriteSet에 추가
			eCurSet = FRAME_WRITE;
		}
		else {
			FD_SET(m_tSockInfo.sock, &ReadSet);		// 해당 소켓에 대해 데이터를 받아야하는 타이밍이다?-> ReadSet에 추가
			eCurSet = FRAME_READ;
		}
	}
	m_eCurSet = eCurSet;
	//switch (eCurSet)
	//{
	//case CClient::FRAME_WRITE:
	//	MakeDebugStringtable("WriteSet");
	//	break;
	//case CClient::FRAME_READ:
	//	MakeDebugStringtable("ReadSet");
	//	break;
	//case CClient::FRAME_END:
	//	MakeDebugStringtable("FinishSet");
	//	break;
	//}

	return true;
}


bool CClient::IsInitializedSoldierMgr()
{
	if ((m_pSoldierMgr) && (m_pSoldierMgr->GetSoldierVecSize() == 0))
		return false;
	else
		return true;
}

void CClient::GetGameStartPacket(S2C_PACKET_GAMESTART& tOutGameStartPacket)
{
	m_pSoldierMgr->GetGameStartPacket(tOutGameStartPacket);
	return;
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

void CClient::MakeDebugStringtable(const char* FuncName)
{
	string Temp;
	Temp = FuncName;

	string statename;
	switch (m_eState)
	{
	case STATE_READY:
		statename = " REady";
		break;
	case STATE_TRANSFORM:
		statename = " Transform";
		break;
	case STATE_INPUT:
		statename = " Input";
		break;
	case STATE_ADDITIONAL:
		statename = " ADDITIONAL";
		break;
	case STATE_END:
	default:
		statename = " Why";
		break;
	}



	Temp += statename;
	m_StringTable.push_back(Temp);
	if (m_StringTable.size() > MAX_DEBUG_STRING_WRITE)
		m_StringTable.pop_front();
}

void CClient::ReserveAdditionalState()
{//input state 일때 예약하러 들어옴.
	//recv할지 결정하자.
	if (m_pPlayer->IsAnyAdditionalData()) 
	{
		m_bReserved_Additional_State[CONDITION_RECV] = true;
	}
	
	//send할지 결정하자.
	if (m_pOtherClient->GetRemAdditionalSize() != 0)
	{
		m_bReserved_Additional_State[CONDITION_SEND] = true;
		m_pOtherClient->CalculateSendAdditionalPacket(m_tSockInfo.cBuf, m_tSockInfo.totalSendLen);
	}

}

void CClient::ClearSendBuffer()
{
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(m_tSockInfo.sock, (struct sockaddr*)&clientaddr, &addrlen);

	m_tSockInfo.totalSendLen = m_tSockInfo.sendbytes = 0;
	if (m_tSockInfo.cBuf != nullptr)
	{
		delete[] m_tSockInfo.cBuf;
		m_tSockInfo.cBuf = nullptr;
	}
}

void CClient::ChangeRecvSoldierTransformCnt(int& iChangeCnt)
{
	if (iChangeCnt == 4)
		iChangeCnt = 5;
	else
		iChangeCnt = 4;
}

void CClient::ConductTransformPacket(bool& bSendTransformDuty, const CPacket& Packet)
{//Recv임.
	//cout << "받아야하는 카운트: " << m_iRecvSoldiersCnt << "  보내야하는 카운트: " <<m_iSendSoldiersCnt << endl;
	size_t size_TransformPacket = sizeof(C2S_PACKET_PLAYER_TRANSFORM);
	size_t size_SoldiersPacket = sizeof(C2S_PACKET_SOLDIER_TRANSFORM) * m_iRecvSoldiersCnt;

	C2S_PACKET_PLAYER_TRANSFORM tPlayerTransform;

	memcpy(&tPlayerTransform, Packet.m_pBuf, size_TransformPacket);//플레이어거 먼저담아갈거임.
	bSendTransformDuty = tPlayerTransform.bRecvTransform;
	C2S_PACKET_SOLDIER_TRANSFORM SoldierTransforms[SOLDIER_MAX_CNT];

	int iRecvIndex = 0;
	if (m_iRecvSoldiersCnt == 5)
		iRecvIndex = 4;//

	Packet.m_pBuf;
	memcpy(&SoldierTransforms[iRecvIndex], Packet.m_pBuf + size_TransformPacket, size_SoldiersPacket);//병사거 담을거임.

	m_pPlayer->SetTransform(tPlayerTransform);
	m_pSoldierMgr->SetSoldiersPacket(SoldierTransforms, iRecvIndex);


	ChangeRecvSoldierTransformCnt(m_iRecvSoldiersCnt);
}

#pragma region Packet


bool CClient::RecvPacket()
{
	MakeDebugStringtable("RecvPacket");

	CPacket tNewPacket(m_eState, m_iRecvSoldiersCnt);
	
	int retval(0);

	if (tNewPacket.m_pBuf != nullptr) { //CPacket의 생성자로 할당 될수있었다면 해당 버퍼에 정보를 담아내야만 한다.
		retval = recv(m_tSockInfo.sock, tNewPacket.m_pBuf, tNewPacket.m_iBufferSize, 0);

		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			return false;
		}
		else if (retval == 0) { 
			return false;
		}
		if (retval != tNewPacket.m_iBufferSize) {
			cout << "받아야할 사이즈: " << tNewPacket.m_iBufferSize << "  받은 패킷양: " << retval << endl;
		}
		string strDebug = "RecvPacket" + to_string(retval);
		MakeDebugStringtable(strDebug.c_str());
		ConductPacket(tNewPacket);
	}
	else if (m_eState == STATE_ADDITIONAL)
	{
		if (m_pPlayer->IsAnyAdditionalData()) {//이제 여기 반드시 트루임.
			pair<int, CMemoryPooler::MemoryBlock*>& Buf = m_pPlayer->GetLastDataBlock();

			retval = recv(m_tSockInfo.sock, Buf.second->pData, Buf.first, 0);	//first는 사이즈 second는 구조체.
			if (retval == SOCKET_ERROR || retval == 0) {
				err_display("recvAdditional()");
				return false;
			}
			int iRemainData = Buf.first - retval;
			if (iRemainData != 0) {// - 여도 문제임 
				cout << "RECV 다 못했음. ADD " << iRemainData << endl;
			}

			m_bReserved_Additional_State[CONDITION_RECV] = false; //다받았다아ㅏ
		}

		ConductPacket(tNewPacket);
	}
	

	return true;
}

bool CClient::SendPacket()
{// 데이터 보내기
	//MakeDebugStringtable("SendPacket");
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
		//cout << "보내야할 사이즈 : " << m_tSockInfo.totalSendLen<< "  받은 패킷양 : " << m_tSockInfo.sendbytes << endl;

		if (m_tSockInfo.totalSendLen == m_tSockInfo.sendbytes) {//SEND ALL COMPLETE
			if (m_eState == STATE_ADDITIONAL)
			{
				int it = 0;
			}
			SendComplete();
		}
	}

	return true;
}

void CClient::SendGameStartPacket()
{	//상대편거 넘겨야함.

	if (m_tSockInfo.totalSendLen != 0)
		return;

	S2C_PACKET_GAMESTART tGameStartPacket;
	if ((m_pOtherClient != nullptr) && (m_pOtherClient->IsInitializedSoldierMgr()))
	{	//상대도 연결됐고			&&	상대한테도 팀정보를 받을수 있다면?
		m_pOtherClient->GetGameStartPacket(tGameStartPacket);
		tGameStartPacket.cGamePlay = m_tSockInfo.eGamePlayTeam;// 이건 색깔 정보니까 내꺼 던져야함. 괜춘.
		m_eReservedState = STATE_TRANSFORM;
	}

	m_tSockInfo.totalSendLen = sizeof(tGameStartPacket);
	m_tSockInfo.cBuf = new char[m_tSockInfo.totalSendLen];

	memcpy(m_tSockInfo.cBuf, &tGameStartPacket, m_tSockInfo.totalSendLen);
}

void CClient::ConductPacket(const CPacket& Packet) //받은 패킷을 set하고, 보낼 패킷의 구조체를 저장해둔다.
{
	//MakeDebugStringtable("ConductPacket");

	switch (m_eState)
	{
	case STATE_READY:
	{
		C2S_PACKET_GAMESTART* pPacket = reinterpret_cast<C2S_PACKET_GAMESTART*>(Packet.m_pBuf);
		if (IsInitializedSoldierMgr() == false)
			m_pSoldierMgr->Initialize(*pPacket);
		else
			m_pSoldierMgr->SetGameStartPacket(*pPacket);
		
		SendGameStartPacket();
		break;
	}
	case STATE_TRANSFORM: 
	{
		bool bSendTransform = false;
		ConductTransformPacket(bSendTransform, Packet);


		if ((bSendTransform) && (m_tSockInfo.sendbytes == 0))// 받아야하는 타이밍이고 보내고있지 않는다면.
		{
			const unsigned long long& size_TransformPacket = sizeof(S2C_PACKET_PLAYER_TRANSFORM);
			const unsigned long long& size_SoldiersPacket = sizeof(C2S_PACKET_SOLDIER_TRANSFORM) * m_iSendSoldiersCnt;
			S2C_PACKET_PLAYER_TRANSFORM tSendPacket = m_pOtherClient->GetOtherPlayerTransform();

			m_tSockInfo.totalSendLen = (int)(size_TransformPacket + size_SoldiersPacket);
			m_tSockInfo.cBuf = new char[m_tSockInfo.totalSendLen];

			memcpy(m_tSockInfo.cBuf, &tSendPacket, size_TransformPacket);

			C2S_PACKET_SOLDIER_TRANSFORM SoldierTransforms[SOLDIER_MAX_CNT];
			m_pOtherClient->GetOtherSoldiersTransform(SoldierTransforms); //여기서 9마리 다받아옴.
			int iSendIndex = 0;
			if (m_iSendSoldiersCnt == 5)
				iSendIndex = 4;
			memcpy(m_tSockInfo.cBuf + size_TransformPacket, &(SoldierTransforms[iSendIndex]), size_SoldiersPacket);

			ChangeRecvSoldierTransformCnt(m_iSendSoldiersCnt);
			//cout << "Send Trans준비완료." << endl;
		}
		else
		{// 보낼 패킷 타이밍이 아니라면 바로 스테이트를 변경해도됨.
			SetClientState(STATE_INPUT);
		}
		break;
	}
	case STATE_INPUT: 
	{
		for (int i = CONDITION_RECV; i < CONDITION_END; i++)
			m_bReserved_Additional_State[i] = false;

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
	case STATE_ADDITIONAL:{	// 이거다시 잡아봐.
		if ((m_tSockInfo.sendbytes == 0))
		{
			if (m_pOtherClient != nullptr) 
			{
				//m_pOtherClient->CalculateSendAdditionalPacket(m_tSockInfo.cBuf, m_tSockInfo.totalSendLen);
			}
		}
		break;
	}

	case STATE_END:
	default:
		break;
	}
}

void CClient::SendComplete(bool doDebugString)
{
	if(doDebugString)
		MakeDebugStringtable("SendComplete");

	string strState;
	switch (m_eState)
	{
	case STATE_READY:
		break;
	case STATE_TRANSFORM:
		SetClientState(STATE_INPUT);
		break;
	case STATE_INPUT: {
		SetClientState(STATE_ADDITIONAL);
		ClearSendBuffer();
		ReserveAdditionalState();
		return;											//여긴 return 임!!!!
	}
	case STATE_ADDITIONAL:
		SetClientState(STATE_TRANSFORM);
		m_bReserved_Additional_State[CONDITION_SEND] = false;  //다보냈다아
		if(m_pOtherClient)
			m_pOtherClient->ClearPlayerUsedData();
		break;
	case STATE_END:
	default:
		break;
	}

	if (m_eReservedState != STATE_END) {
		SetClientState(m_eReservedState);
		m_eReservedState = STATE_END;
	}


	ClearSendBuffer();

}

#pragma endregion Packet