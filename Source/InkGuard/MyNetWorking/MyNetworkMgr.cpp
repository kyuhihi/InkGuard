#include "MyNetworkMgr.h"
#include "../InkGuard.h"
#include "../CustomFunctional.h"

MyNetworkMgr* MyNetworkMgr::m_pInstance = nullptr;

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

	m_tClientSock.bConnectSuccess = true;

}

void MyNetworkMgr::Tidy()
{// 소켓 닫기
	closesocket(m_tClientSock.sock);
	m_tClientSock.bConnectSuccess = false;

	// 윈속 종료
	WSACleanup();
}

#pragma region Packet

#pragma region SendPlayerTransform
const GAME_PLAY MyNetworkMgr::RecvGameStart()
{
	if (!m_tClientSock.bConnectSuccess)
		return GAME_END;

	S2C_PACKET_PLAYER_TRANSFORM tGameStartPacket;
	
	int retval{ 0 };
	retval = recv(m_tClientSock.sock, (char*)&tGameStartPacket, sizeof(tGameStartPacket), MSG_WAITALL);
	if ((retval == SOCKET_ERROR) || (retval != sizeof(tGameStartPacket))) {
		err_quit("RecvGameStart");
		Tidy();
		return GAME_END;
	}

	return (GAME_PLAY)tGameStartPacket.cGamePlay;
}

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
	if ((retval == SOCKET_ERROR) || 
		(retval != sizeof(S2C_PACKET_PLAYER_TRANSFORM))||
		(tOutPacket.cGamePlay == GAME_PLAY::GAME_END)) {
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