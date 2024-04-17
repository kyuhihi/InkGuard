#include "MyNetworkMgr.h"
#include "../InkGuard.h"
#include "../Customs/CustomFunctional.h"
#include "../InkGuardGameMode.h"
#include "../Spawner/SpawnMgr.h"
#include "../MyBlueprintFunctionLibrary.h"

MyNetworkMgr* MyNetworkMgr::m_pInstance = nullptr;

SOLDIERINFO MyNetworkMgr::m_tSoldierInfo[SOLDIER_MAX_CNT];
SOLDIERINFO MyNetworkMgr::m_tOtherSoldierInfo[SOLDIER_MAX_CNT];

#define MAX_ADDITIONAL_SIZE 100

MyNetworkMgr::MyNetworkMgr()
{
	Initialize(); //네트워킹 커넥트 작업.
	ASpawnMgr::Initialize();
}

MyNetworkMgr::~MyNetworkMgr()
{
	Tidy();
}

void MyNetworkMgr::Initialize()
{
	InitializeSocket();
	InitializeAdditionalList();
}

void MyNetworkMgr::InitializeSocket()
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
}

void MyNetworkMgr::InitializeAdditionalList()
{
	m_SendAdditionalPacketVec.reserve(MAX_ADDITIONAL_SIZE); // 최적화..
	m_RecvAdditionalPacketVec.reserve(MAX_ADDITIONAL_SIZE);

	for (int i = 0; i < MAX_ADDITIONAL_SIZE; ++i)
	{
		AdditionalPacket newPacket;
		m_RecvAdditionalPacketVec.push_back(newPacket);
	}
	for (int i = 0; i < MAX_ADDITIONAL_SIZE; ++i)
	{
		AdditionalPacket newPacket;
		m_SendAdditionalPacketVec.push_back(newPacket);
	}
}

void MyNetworkMgr::Tidy()
{// 소켓 닫기
	ClearAdditionalPacket();

	closesocket(m_tClientSock.sock);
	m_tClientSock.bConnectSuccess = false;

	// 윈속 종료
	WSACleanup();

	m_RecvAdditionalPacketVec.clear();
	m_SendAdditionalPacketVec.clear();
	m_pSpawnMgr = nullptr;

}

void MyNetworkMgr::SetSoldierInfo(int iIndex, int iSoldierType, int iTargetTerritory)
{
	if (SOLDIER_MAX_CNT <= iSoldierType || iSoldierType < 0)
		return;

	ASpawnMgr::SetSoldierInfo(iIndex, iSoldierType, iTargetTerritory); //우리팀 셋팅

}

void MyNetworkMgr::SetReservedOpenLevel(bool bNewValue)
{
	m_bReservedOpenLevel = bNewValue;
}

void MyNetworkMgr::ClearAdditionalPacket()// 패킷다 받고 변수들 상태만 바꿔놓는 함수.
{
	for (auto& iter: m_RecvAdditionalPacketVec)
	{
		if (iter.bUse) {
			iter.bUse = false;
			iter.iDataSize = 0;
			continue;
		}
		break;
	}


	for (auto& iter : m_SendAdditionalPacketVec) {
		if (iter.bUse) {
			iter.bUse = false;
			iter.iDataSize = 0;
			continue;
		}
		break;
	}
	
}

bool MyNetworkMgr::RequestRemainVectorIndex(bool bSendVec, int& iOutVectorIndex)
{
	bool bCanReturn(false);

	if (bSendVec) {
		for (int i = 0; i < m_SendAdditionalPacketVec.size(); ++i) {
			if (m_SendAdditionalPacketVec[i].bUse)// 이미사용하고있는 벡터라면 컨티뉴
				continue;

			bCanReturn = true;
			iOutVectorIndex = i;
		}
	}
	else {
		for (int i = 0; i < m_RecvAdditionalPacketVec.size(); ++i) {
			if (m_RecvAdditionalPacketVec[i].bUse)// 이미사용하고있는 벡터라면 컨티뉴
				continue;

			bCanReturn = true;
			iOutVectorIndex = i;
		}
	}


	return bCanReturn;
}

#pragma region Packet

#pragma region SendGameStart
void MyNetworkMgr::SendGameStart()
{
	if (!m_tClientSock.bConnectSuccess)
		return;

	C2S_PACKET_GAMESTART tNewPacket;
	for (int i = 0; i < SOLDIER_MAX_CNT; i++) {
		tNewPacket.cSoldierInfo[i] = m_tSoldierInfo[i].eSoldierType;
		tNewPacket.cTargetTerritory[i] = m_tSoldierInfo[i].eTargetTerritory;
	}

	int retval{ 0 };
	retval = send(m_tClientSock.sock, reinterpret_cast<char*>(&tNewPacket), sizeof(tNewPacket), 0);
	if ((retval == SOCKET_ERROR) || (retval != sizeof(tNewPacket)))
	{
		err_quit("SendGameStart");
		Tidy();
		return;
	}
}

void MyNetworkMgr::RecvGameStart()
{//인자로 클라이언트 소켓이 넘어옴.
	if (!m_tClientSock.bConnectSuccess)
		return;

	S2C_PACKET_GAMESTART tGameStartPacket;

	int retval{ 0 };
	retval = recv(m_tClientSock.sock, (char*)&tGameStartPacket, sizeof(S2C_PACKET_GAMESTART), MSG_WAITALL);
	if ((retval == SOCKET_ERROR) || (retval != sizeof(tGameStartPacket))) {
		err_quit("RecvGameStart");
		MyNetworkMgr::GetInstance()->Tidy();
		return;
	}

	GAME_PLAY eGamePacketType = (GAME_PLAY)tGameStartPacket.cGamePlay; //락을 잡아야겠쥐?
	if (eGamePacketType == GAME_END)  //게임 시작을 안한 것.
		return;

	m_bGameStart = true;					//이거 켜지면 게임 시작인거.

	m_eGameTeam = eGamePacketType;
	SetReservedOpenLevel(true);		//오픈 레벨 예약이 되었습니까? 예.

	ASpawnMgr::SetOtherSoldierInfo(tGameStartPacket); //적팀것 셋팅

	return;
}
#pragma endregion

#pragma region SendPlayerTransform

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

#pragma endregion

#pragma region SendInput
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

	ClearAdditionalPacket();

	unsigned long long llPacketSize(sizeof(S2C_PACKET_PLAYER_INPUT));

	int retval{ 0 };
	retval = recv(m_tClientSock.sock, (char*)&tOutPacket, llPacketSize, MSG_WAITALL);
	if ((retval == SOCKET_ERROR) || (retval != llPacketSize))
	{
		err_quit("RecvPlayerInputData");
		Tidy();
		return false;
	}

	//if (tOutPacket.sAdditionalPacketSize != 0) // 추가로 받아야하는 패킷이있다면 버퍼 할당함..
	//{
	//	m_pRecvAdditionalPacket = new char[tOutPacket.sAdditionalPacketSize];
	//	ZeroMemory(m_pRecvAdditionalPacket, tOutPacket.sAdditionalPacketSize);

	//	m_iRecvAdditionalPacketSize = tOutPacket.sAdditionalPacketSize;
	//}
	//else
	//{
	//	m_iRecvAdditionalPacketSize = 0;
	//}


	return true;
}


#pragma endregion

#pragma region Send Additional

void MyNetworkMgr::AppendDataToAdditionalList(bool bSendVec, EAdditionalPacketType eNewType, const C2S_PACKET_ADDITIONAL_FLOAT3x3 tNewPacket)
{
	int iRemainVectorIndex = -1;
	if (RequestRemainVectorIndex(bSendVec, iRemainVectorIndex))
	{
		int iNewPacketSize = sizeof(C2S_PACKET_ADDITIONAL_FLOAT3x3);
		if (bSendVec)
		{
			m_SendAdditionalPacketVec[iRemainVectorIndex].bUse = true;
			m_SendAdditionalPacketVec[iRemainVectorIndex].ePacketType = eNewType;
			m_SendAdditionalPacketVec[iRemainVectorIndex].iDataSize = iNewPacketSize;
			memcpy(m_SendAdditionalPacketVec[iRemainVectorIndex].pData,&tNewPacket, iNewPacketSize);
		}
		else
		{
			m_RecvAdditionalPacketVec[iRemainVectorIndex].bUse = true;
			m_RecvAdditionalPacketVec[iRemainVectorIndex].ePacketType = eNewType;
			m_RecvAdditionalPacketVec[iRemainVectorIndex].iDataSize = iNewPacketSize;
			memcpy(m_RecvAdditionalPacketVec[iRemainVectorIndex].pData, &tNewPacket, iNewPacketSize);
		}
	}
}

void MyNetworkMgr::SendAdditionalData()
{
	for (auto& iter : m_SendAdditionalPacketVec)
	{
		if (iter.bUse == false)
			break;

		//여기서 패킷 복사할것. TODO Recv도 만들고, 서버도 받고 보낼준비하고.........저
	}
}

#pragma endregion


#pragma endregion