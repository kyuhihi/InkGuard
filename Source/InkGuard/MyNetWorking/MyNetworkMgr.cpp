#include "MyNetworkMgr.h"
#include "../InkGuard.h"
#include "../Customs/CustomFunctional.h"
#include "../InkGuardGameMode.h"
#include "../Spawner/SpawnMgr.h"
#include "../MyBlueprintFunctionLibrary.h"

MyNetworkMgr* MyNetworkMgr::m_pInstance = nullptr;

SOLDIERINFO MyNetworkMgr::m_tSoldierInfo[SOLDIER_MAX_CNT];
SOLDIERINFO MyNetworkMgr::m_tOtherSoldierInfo[SOLDIER_MAX_CNT];
string MyNetworkMgr::SERVERIP = "192.168.25.55";

#define MAX_ADDITIONAL_SIZE 128

MyNetworkMgr::MyNetworkMgr()
{
	//Initialize(); //��Ʈ��ŷ Ŀ��Ʈ �۾�.
	//ASpawnMgr::Initialize();
}

MyNetworkMgr::~MyNetworkMgr()
{
	//Tidy();
}

void MyNetworkMgr::Initialize()
{
	InitializeSocket();
	InitializeAdditionalList();
}

void MyNetworkMgr::InitializeSocket()
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	// ���� ����
	m_tClientSock.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_tClientSock.sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP.c_str(), &serveraddr.sin_addr);
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
	m_SendAdditionalPacketVec.reserve(MAX_ADDITIONAL_SIZE); // ����ȭ..
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
{// ���� �ݱ�
	ClearAdditionalPacket();

	closesocket(m_tClientSock.sock);
	m_tClientSock.bConnectSuccess = false;

	// ���� ����
	WSACleanup();

	m_RecvAdditionalPacketVec.clear();
	m_SendAdditionalPacketVec.clear();
	m_pSpawnMgr = nullptr;

	ofstream outFile;

	outFile.open("InkGuardNetworkLog.txt");

	if (!outFile.is_open()) { // ������ ������ ���� ���
		return;
	}

	// ����Ʈ�� �� ��Ҹ� ���Ͽ� �ۼ�
	for (const auto& str : m_DebugStringTable) {
		outFile << str << std::endl;
	}

	outFile.close(); // ���� �ݱ�

}

void MyNetworkMgr::SetSoldierInfo(int iIndex, int iSoldierType, int iTargetTerritory)
{
	if (SOLDIER_MAX_CNT <= iSoldierType || iSoldierType < 0)
		return;

	ASpawnMgr::SetSoldierInfo(iIndex, iSoldierType, iTargetTerritory); //�츮�� ����

}

void MyNetworkMgr::SetReservedOpenLevel(bool bNewValue)
{
	m_bReservedOpenLevel = bNewValue;
}

void MyNetworkMgr::ClearAdditionalPacket()// ��Ŷ�� �ް� ������ ���¸� �ٲ���� �Լ�.
{
	m_sRecvAdditionalPacketSize = m_sSendAdditionalPacketSize = 0;
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
			if (m_SendAdditionalPacketVec[i].bUse)// �̹̻���ϰ��ִ� ���Ͷ�� ��Ƽ��
				continue;

			bCanReturn = true;
			iOutVectorIndex = i;
		}
	}
	else {
		for (int i = 0; i < m_RecvAdditionalPacketVec.size(); ++i) {
			if (m_RecvAdditionalPacketVec[i].bUse)// �̹̻���ϰ��ִ� ���Ͷ�� ��Ƽ��
				continue;

			bCanReturn = true;
			iOutVectorIndex = i;
		}
	}


	return bCanReturn;
}

void MyNetworkMgr::ChangeSendSoldierTransformCnt(int& iChangeCnt)
{
	if (iChangeCnt == 4)
		iChangeCnt = 5;
	else
		iChangeCnt = 4;
}

void MyNetworkMgr::MakeDebugStringTable(const char* pString)
{
	m_DebugStringTable.push_back(pString);

	if(m_DebugStringTable.size() > 10)
		m_DebugStringTable.pop_front();
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
{//���ڷ� Ŭ���̾�Ʈ ������ �Ѿ��.
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

	GAME_PLAY eGamePacketType = (GAME_PLAY)tGameStartPacket.cGamePlay; //���� ��ƾ߰���?
	if (eGamePacketType == GAME_END)  //���� ������ ���� ��.
		return;

	m_bGameStart = true;					//�̰� ������ ���� �����ΰ�.

	m_eGameTeam = eGamePacketType;
	SetReservedOpenLevel(true);		//���� ���� ������ �Ǿ����ϱ�? ��.

	ASpawnMgr::SetOtherSoldierInfo(tGameStartPacket); //������ ����

	/*for (int i = 0; i < SOLDIER_MAX_CNT; i++)
	{
		UE_LOG(InkGuardNetErr, Warning, TEXT("Other Soldier: %d        Target: %d"), m_tOtherSoldierInfo[i].eSoldierType, m_tOtherSoldierInfo[i].eTargetTerritory);
	}*/

	return;
}
#pragma endregion

#pragma region SendPlayerTransform

void MyNetworkMgr::SendPlayerTransform(const FVector& vPlayerPosition, const FRotator& vPlayerRotation, const float& fVelocityZ, const float& fSpeed, const float& fBakuHP)
{
	if (!m_tClientSock.bConnectSuccess)
		return;

	const unsigned long long& size_TransformPacket = sizeof(C2S_PACKET_PLAYER_TRANSFORM);
	const unsigned long long& size_SoldiersPacket = sizeof(C2S_PACKET_SOLDIER_TRANSFORM) * m_iSendSoldierCnt;
	const unsigned long long size_Total= size_TransformPacket + size_SoldiersPacket;

	C2S_PACKET_PLAYER_TRANSFORM tTransformPacket;
	tTransformPacket.vPosition = UCustomFunctional::FVector_To_float3(vPlayerPosition);
	tTransformPacket.fYaw = vPlayerRotation.Yaw;
	tTransformPacket.fSpeed = fSpeed;
	tTransformPacket.fVelocityZ = fVelocityZ;
	tTransformPacket.fHP = fBakuHP;
	tTransformPacket.bRecvTransform = m_bSyncTime;

	C2S_PACKET_SOLDIER_TRANSFORM SoldierTransforms[SOLDIER_MAX_CNT];
	m_pSpawnMgr->GetSoldierData(SoldierTransforms); //���� ä�� �ð���.
	
	char* SendBuffer = new char[size_Total];
	memcpy(SendBuffer, &tTransformPacket, size_TransformPacket);//�÷��̾�� ������ư�����.

	int iSendStartIndex = 0;
	if (m_iSendSoldierCnt == 5)
		iSendStartIndex = 4;//�ٵ� 4�� �϶��� �տ�, 5���϶� ��... �����ȴ�.
	memcpy(SendBuffer + size_TransformPacket, &(SoldierTransforms[iSendStartIndex]), size_SoldiersPacket);//����� ��������.

	int retval{ 0 };
	retval = send(m_tClientSock.sock, SendBuffer, size_Total, 0);
	if ((retval == SOCKET_ERROR) || (retval != size_Total))
	{
		err_quit("SendPlayerTransform");
		Tidy();
		delete[] SendBuffer;

		return;
	}
	MakeDebugStringTable(to_string(retval).c_str());

	delete[] SendBuffer;
	ChangeSendSoldierTransformCnt(m_iSendSoldierCnt);
}

bool MyNetworkMgr::RecvPlayerTransform(S2C_PACKET_PLAYER_TRANSFORM& tOutPacket)
{
	if (!m_tClientSock.bConnectSuccess || !m_bSyncTime)
		return false;

	const unsigned long long& size_TransformPacket = sizeof(S2C_PACKET_PLAYER_TRANSFORM);
	const unsigned long long& size_SoldiersPacket = sizeof(C2S_PACKET_SOLDIER_TRANSFORM) * m_iRecvSoldierCnt;
	const unsigned long long size_Total = size_TransformPacket + size_SoldiersPacket;
	char* RecvBuffer = new char[size_Total];

	int retval{ 0 };
	retval = recv(m_tClientSock.sock, RecvBuffer, size_Total, MSG_WAITALL);
	if ((retval == SOCKET_ERROR) || (retval != size_Total)) {
		err_quit("RecvPlayerTransform");
		Tidy();
		delete[] RecvBuffer;
		return false;
	}

	memcpy(&tOutPacket, RecvBuffer, size_TransformPacket);//�÷��̾�� ������ư�����.
	C2S_PACKET_SOLDIER_TRANSFORM SoldierTransforms[SOLDIER_MAX_CNT];

	int iRecvIndex = 0;
	if (m_iRecvSoldierCnt == 5)
		iRecvIndex = 4;//�ٵ� 4�� �϶��� �տ�, 5���϶� ��... �����ȴ�.

	memcpy(&SoldierTransforms[iRecvIndex], RecvBuffer + size_TransformPacket, size_SoldiersPacket);//����� ��������.

	m_pSpawnMgr->SetSoldierData(SoldierTransforms, iRecvIndex);

	delete[] RecvBuffer;

	ChangeSendSoldierTransformCnt(m_iRecvSoldierCnt);
	return true;
}

#pragma endregion

#pragma region SendInput
void MyNetworkMgr::SendPlayerInputData(C2S_PACKET_PLAYER_INPUT& tBakuInputData)
{
	if (!m_tClientSock.bConnectSuccess)
		return;

	tBakuInputData.sAdditionalPacketSize = m_sSendAdditionalPacketSize; // ������ �߰��� ���� ��Ŷ ����� �־��ش�.

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

	m_sRecvAdditionalPacketSize = tOutPacket.sAdditionalPacketSize;

	//if (m_sRecvAdditionalPacketSize != 0) // �߰��� �޾ƾ��ϴ� ��Ŷ���ִٸ� recv�ѹ����� �ؾ���.
	//{
	//	RecvAdditionalData();
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
			
			
			m_sSendAdditionalPacketSize += sizeof(char) + iNewPacketSize;
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

int MyNetworkMgr::SendAdditionalData()
{
	if (m_sSendAdditionalPacketSize <= 0)
		return -1;

	char* SendBuffer = new char[m_sSendAdditionalPacketSize];
	int iOffset(0);
	int sizeOfChar(sizeof(char));

	for (auto& Packet : m_SendAdditionalPacketVec)
	{
		if (Packet.bUse == false)
			break;

		char cPacketType = (char)Packet.ePacketType;			//��ŶŸ�� �־��ְ�
		memcpy(SendBuffer + iOffset, &cPacketType, sizeOfChar);	//char��ŭ �����ϰ�
		iOffset += sizeOfChar;//������ �����ְ�

		memcpy(SendBuffer + iOffset, &Packet.pData, sizeof(Packet.iDataSize));//������ �� �ڿ� �־��ְ�
		iOffset += Packet.iDataSize;// ������ �� �����ְ�
	}


	int retval{ 0 };
	retval = send(m_tClientSock.sock, SendBuffer, m_sSendAdditionalPacketSize, 0);
	if ((retval == SOCKET_ERROR) || (retval != m_sSendAdditionalPacketSize))
	{
		err_quit("SendAdditionalData");
		Tidy();
		return -1;
	}
	string strSend = "SendAdditionalData "+ to_string(retval);
	MakeDebugStringTable(strSend.c_str());
	

	delete[] SendBuffer;
	SendBuffer = nullptr;

	return retval;
}

void MyNetworkMgr::RecvAdditionalData()
{
	if (m_sRecvAdditionalPacketSize <= 0) 
		return;

	char* RecvPacket = new char[m_sRecvAdditionalPacketSize];

	int retval{ 0 };

	retval = recv(m_tClientSock.sock, RecvPacket, m_sRecvAdditionalPacketSize, MSG_WAITALL);
	if ((retval == SOCKET_ERROR) || (retval != m_sRecvAdditionalPacketSize))
	{
		err_quit("RecvAdditionalData");
		Tidy();
		return;
	}
	ConductAdditionalData(RecvPacket);

}

void  MyNetworkMgr::FindAdditionalData(EAdditionalPacketType eFindType, C2S_PACKET_ADDITIONAL_FLOAT3x3& tOutData)
{
	for (auto& iter : m_RecvAdditionalPacketVec)
	{
		if (iter.bUse == false)
			return;
		if (iter.ePacketType == eFindType)
		{
			memcpy(&tOutData, iter.pData, sizeof(C2S_PACKET_ADDITIONAL_FLOAT3x3));
		}
	}
}

void MyNetworkMgr::ConductAdditionalData(const char* pNewPacket)
{
	int iOffset(0);
	
	int iSizeofchar(sizeof(char));
	while (iOffset < m_sRecvAdditionalPacketSize)
	{//���ۿ��� char �� ��ŶŸ�� �����ϰ�, �� ������ ������.
		
		char cpyPacketType(0);
		memcpy(&cpyPacketType, pNewPacket + iOffset, iSizeofchar);
		iOffset += iSizeofchar;

		EAdditionalPacketType eNewPacketType((EAdditionalPacketType)cpyPacketType);

		C2S_PACKET_ADDITIONAL_FLOAT3x3 tFloat3x3;

		switch (eNewPacketType)
		{
			case EAdditionalPacketType::ADD_VAULT: 
			{
				memcpy(&tFloat3x3, pNewPacket + iOffset, sizeof(C2S_PACKET_ADDITIONAL_FLOAT3x3));
				iOffset += sizeof(C2S_PACKET_ADDITIONAL_FLOAT3x3);
				AppendDataToAdditionalList(false, eNewPacketType, tFloat3x3);

				break; 
			}
			default: 
			{
				break;
			}
		}

	}
	m_sRecvAdditionalPacketSize = 0;
}




#pragma endregion


#pragma endregion