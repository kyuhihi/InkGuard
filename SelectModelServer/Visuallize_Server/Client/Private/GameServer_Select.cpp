#include "stdafx.h"
#include "GameServer_Select.h"

CGameServer_Select::CGameServer_Select()
{
}

CGameServer_Select::~CGameServer_Select()
{
    Tidy();
}

void CGameServer_Select::ServerLoop()
{
    int retval;

    for (int i = 0; i < FD_SETSIZE; ++i)
        ClientArray[i] = nullptr;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return;

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVER_PORT);
    retval = ::bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    u_long on = 1;
    retval = ioctlsocket(listen_sock, FIONBIO, &on);
    if (retval == SOCKET_ERROR) err_display("ioctlsocket()");

    fd_set ReadSet, WriteSet;
    int nready;
    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;

    printf("클라이언트로부터의 접속을 대기중입니다\n");

    while (1) {

        m_DoneMtx.lock();
        if (m_bDone)
            break;
        m_DoneMtx.unlock();

        FD_ZERO(&ReadSet);
        FD_ZERO(&WriteSet);
        FD_SET(listen_sock, &ReadSet);

        for (int i = 0; i < nTotalSockets; i++) {
            if (ClientArray[i]->PutInReadOrWriteSet(ReadSet, WriteSet) == false)
                ClientArray[i]->PutInReadOrWriteSet(ReadSet, WriteSet);
        }

        nready = select(0, &ReadSet, &WriteSet, NULL, NULL);
        if (nready == SOCKET_ERROR) err_quit("select()");



        if (FD_ISSET(listen_sock, &ReadSet)) {
            addrlen = sizeof(clientaddr);
            client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
            if (client_sock == INVALID_SOCKET) {
                err_display("accept()");
                break;
            }
            else {
                char addr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
                printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
                    addr, ntohs(clientaddr.sin_port));
                if (!AddSocketInfo(client_sock))
                    closesocket(client_sock);
            }
            if (--nready <= 0)
                continue;
        }

        for (int i = 0; i < nTotalSockets; i++) {
            const CClient::SOCKETINFO* ptr = ClientArray[i]->GetSocketInfo();
            if (FD_ISSET(ptr->sock, &ReadSet)) {
                if (ClientArray[i]->RecvPacket() == false)
                    RemoveSocketInfo(i);
            }
            else if (FD_ISSET(ptr->sock, &WriteSet)) {
                if (ClientArray[i]->SendPacket() == false)
                    RemoveSocketInfo(i);
            }
        }
    }

    Tidy();

}

void CGameServer_Select::SetbDone()
{
    m_DoneMtx.lock();
    m_bDone = true;
    Tidy();
    m_DoneMtx.unlock();
}

void CGameServer_Select::Tidy()
{	// 윈속 종료
    for (int i = 0; i < nTotalSockets; i++) {
        const CClient::SOCKETINFO* ptr = ClientArray[i]->GetSocketInfo();
        RemoveSocketInfo(i);
    }

    WSACleanup();

    closesocket(listen_sock);

    CMemoryPooler::GetInstance()->DestroyInstance();
}

// 소켓 정보 추가
bool CGameServer_Select::AddSocketInfo(SOCKET sock)
{
    if (nTotalSockets >= FD_SETSIZE) {
        printf("[오류] 소켓 정보를 추가할 수 없습니다!\n");
        return false;
    }

    CClient* ptr = new CClient(sock);
    if (ptr == NULL) {
        printf("[오류] 메모리가 부족합니다!\n");
        return false;
    }

    ClientArray[nTotalSockets++] = ptr;
    MatchMakingMgr.AddClient(ptr);

    return true;
}

// 소켓 정보 삭제
void CGameServer_Select::RemoveSocketInfo(int nIndex)
{
	const CClient::SOCKETINFO* ptr = ClientArray[nIndex]->GetSocketInfo();

	MatchMakingMgr.RemoveClient(ClientArray[nIndex]);

	delete ClientArray[nIndex];
	ClientArray[nIndex] = nullptr;

	if (nIndex != (nTotalSockets - 1))
		ClientArray[nIndex] = ClientArray[nTotalSockets - 1];
	--nTotalSockets;
}

