#include "MyNetworkMgr.h"
#include "../InkGuard.h"
#include "../CustomFunctional.h"

MyNetworkMgr* MyNetworkMgr::m_pInstance = nullptr;

mutex MyNetworkMgr::m_GameStartMutex;
GAME_PLAY MyNetworkMgr::m_eGameTeam = GAME_PLAY::GAME_END;
bool MyNetworkMgr::m_bGameStart = false;
SOLDIERINFO MyNetworkMgr::m_tSoldierInfo[SOLDIER_MAX_CNT]; //내꺼 솔져 정보.

MyNetworkMgr::MyNetworkMgr()
{
	Initialize();
}

MyNetworkMgr::~MyNetworkMgr()
{
	Tidy();
}

void MyNetworkMgr::Initialize()
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	// 소켓 생성
	m_tClientSock.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_tClientSock.sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVER_PORT);
	retval = connect(m_tClientSock.sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	
	if (retval == SOCKET_ERROR) {
		err_quit("connect()");
		Tidy();
		return;
	}
	m_tClientSock.bConnectSuccess = true;

	for (int i = 0; i < SOLDIER_MAX_CNT; ++i) {
		m_tSoldierInfo[i].eSoldierType = (SOLDIER_TYPE)(i / 2);
		m_tSoldierInfo[i].eTargetTerritory = (TERRITORY_TYPE)(i / 3);
	}
}

void MyNetworkMgr::Tidy()
{// 소켓 닫기
	CloseHandle(m_hThread);

	closesocket(m_tClientSock.sock);
	m_tClientSock.bConnectSuccess = false;

	m_GameStartMutex.unlock();
	// 윈속 종료
	WSACleanup();
}

DWORD WINAPI MyNetworkMgr::RecvGameStart(LPVOID arg)
{//인자로 클라이언트 소켓이 넘어옴.
	SOCKET client_sock = (SOCKET)arg;

	S2C_PACKET_GAMESTART tGameStartPacket;

	int retval{ 0 };
	retval = recv(client_sock, (char*)&tGameStartPacket, sizeof(tGameStartPacket), MSG_WAITALL);
	if ((retval == SOCKET_ERROR) || (retval != sizeof(tGameStartPacket))) {
		err_quit("RecvGameStart");
		MyNetworkMgr::GetInstance()->Tidy();
		return 0;
	}

	m_GameStartMutex.lock();
	
	m_eGameTeam = (GAME_PLAY)tGameStartPacket.cGamePlay; //락을 잡아야겠쥐?
	m_bGameStart = true;

	for (int i = 0; i < SOLDIER_MAX_CNT; ++i)
	{//받은 상대편 패킷으로 이제 생성해야함.

	}

	m_GameStartMutex.unlock();
	
	return 0;
}

void MyNetworkMgr::SetSoldierInfo(int iIndex, int iSoldierType, int iTargetTerritory)
{
	if (SOLDIER_MAX_CNT < iSoldierType || iSoldierType < 0)
		return;

	m_tSoldierInfo[iIndex].eSoldierType = (SOLDIER_TYPE)iSoldierType;
	m_tSoldierInfo[iIndex].eTargetTerritory = (TERRITORY_TYPE)iTargetTerritory;
}

#pragma region Packet

void MyNetworkMgr::OpenMainGame() //내 솔져 정보들을 보내고, 상대편의 솔져정보들을 받는 쓰레드를 생성하고, 씬을 오픈시킴.
{
	C2S_PACKET_GAMESTART tSendPacket;
	for (int i = 0; i < SOLDIER_MAX_CNT; ++i)
	{
		tSendPacket.cSoldierInfo[i] = m_tSoldierInfo[i].eSoldierType;
		tSendPacket.cTargetTerritory[i] = m_tSoldierInfo[i].eTargetTerritory;
	}


	int retval{ 0 };
	retval = send(m_tClientSock.sock, reinterpret_cast<char*>(&tSendPacket), sizeof(tSendPacket), 0);
	if ((retval == SOCKET_ERROR) || (retval != sizeof(tSendPacket)))
	{
		err_quit("OpenMainGame Before Thread Creating");
		Tidy();
		return;
	}

	m_hThread = CreateThread(NULL, 0, RecvGameStart, (LPVOID)m_tClientSock.sock, 0, NULL);
}

#pragma region SendPlayerTransform
void MyNetworkMgr::SendPlayerTransform(C2S_PACKET_PLAYER_TRANSFORM tNewTransform)
{
	if (!m_tClientSock.bConnectSuccess)
		return;

	const unsigned long long& tPacketSize = sizeof(tNewTransform);

	int retval{ 0 };
	retval = send(m_tClientSock.sock, reinterpret_cast<char*>(&tNewTransform), sizeof(tPacketSize), 0);
	if ((retval == SOCKET_ERROR) || (retval != tPacketSize))
	{
		err_quit("SendPlayerTransform");
		Tidy();
		return;
	}

}

void MyNetworkMgr::SendPlayerTransform(const FVector& vPlayerPosition, const FRotator& vPlayerRotation, const float& fVelocityZ, const float& fSpeed)
{
	if (!m_tClientSock.bConnectSuccess)
		return;

	C2S_PACKET_PLAYER_TRANSFORM tTransformPacket;
	const unsigned long long& tPacketSize = sizeof(tTransformPacket);

	ZeroMemory(&tTransformPacket, tPacketSize);

	tTransformPacket.vPosition = UCustomFunctional::FVector_To_float3(vPlayerPosition);
	tTransformPacket.fYaw = vPlayerRotation.Yaw;
	tTransformPacket.fSpeed = fSpeed;
	tTransformPacket.fVelocityZ = fVelocityZ;
	tTransformPacket.bRecvTransform = m_bSyncTime;

	int retval{ 0 };
	retval = send(m_tClientSock.sock, reinterpret_cast<char*>(&tTransformPacket), tPacketSize, 0);
	if ((retval == SOCKET_ERROR) || (retval != tPacketSize))
	{
		err_quit("SendPlayerTransform");
		Tidy();
		return;
	}

}

bool MyNetworkMgr::RecvPlayerTransform(S2C_PACKET_PLAYER_TRANSFORM& tOutPacket)
{
	if (!m_tClientSock.bConnectSuccess || !m_bSyncTime)
		return false;

	int retval{ 0 };
	retval = recv(m_tClientSock.sock, (char*)&tOutPacket, sizeof(S2C_PACKET_PLAYER_TRANSFORM), MSG_WAITALL);
	if ((retval == SOCKET_ERROR) || (retval != sizeof(S2C_PACKET_PLAYER_TRANSFORM))) {
		err_quit("RecvPlayerTransform");
		Tidy();
		return false;
	}

	return true;
}

void MyNetworkMgr::SendPlayerInputData(C2S_PACKET_PLAYER_INPUT& tBakuInputData)
{
	if (!m_tClientSock.bConnectSuccess)
		return;
	int retval{ 0 };
	unsigned long long llPacketSize(sizeof(tBakuInputData));
	retval = send(m_tClientSock.sock, reinterpret_cast<char*>(&tBakuInputData), llPacketSize, 0);
	if ((retval == SOCKET_ERROR) || (retval != llPacketSize))
	{
		err_quit("SendPlayerInputData");
		Tidy();
		return;
	}
}

bool MyNetworkMgr::RecvPlayerInputData(S2C_PACKET_PLAYER_INPUT& tOutPacket)
{
	if (!m_tClientSock.bConnectSuccess)
		return false;

	unsigned long long llPacketSize(sizeof(S2C_PACKET_PLAYER_INPUT));

	int retval{ 0 };
	retval = recv(m_tClientSock.sock, (char*)&tOutPacket, llPacketSize, MSG_WAITALL);
	if ((retval == SOCKET_ERROR) || (retval != llPacketSize))
	{
		err_quit("RecvPlayerInputData");
		Tidy();
		return false;
	}


	return true;
}

#pragma endregion



#pragma endregion