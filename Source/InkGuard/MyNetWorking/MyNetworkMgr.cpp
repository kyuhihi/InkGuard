#include "MyNetworkMgr.h"
#include "../InkGuard.h"

MyNetworkMgr* MyNetworkMgr::m_pInstance = nullptr;

MyNetworkMgr::MyNetworkMgr()
{
	//Initialize();
}

MyNetworkMgr::~MyNetworkMgr()
{
	//Tidy();
}

void MyNetworkMgr::Initialize()
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	// 소켓 생성
	m_ClientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ClientSock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVER_PORT);
	retval = connect(m_ClientSock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	
	return;
}

void MyNetworkMgr::Tidy()
{// 소켓 닫기
	closesocket(m_ClientSock);

	// 윈속 종료
	WSACleanup();
}

