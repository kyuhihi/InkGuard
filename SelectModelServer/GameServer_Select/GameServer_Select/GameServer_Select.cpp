#include "Include.h"
#include "Client.h"
#include "MatchMakingMgr.h"

int nTotalSockets = 0;
CClient* ClientArray[FD_SETSIZE];// 총 64개의 소켓을 받을수있다.
CMatchMakingMgr MatchMakingMgr;

// 소켓 정보 관리 함수
bool AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);

int main(int argc, char* argv[])
{
	int retval;
#pragma region 윈속 초기화
	for (int i = 0; i < FD_SETSIZE; ++i)
		ClientArray[i] = nullptr;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	struct sockaddr_in serveraddr;
	memset(&			serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 넌블로킹 소켓으로 전환
	u_long on = 1;
	retval = ioctlsocket(listen_sock, FIONBIO, &on);
	if (retval == SOCKET_ERROR) err_display("ioctlsocket()");

	// 데이터 통신에 사용할 변수
	fd_set ReadSet, WriteSet; // 읽기 Set, 쓰기 Set 준비. 예외셋은 필요없음.
	int nready;
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	
	printf("클라이언트로부터의 접속을 대기중입니다\n");

#pragma endregion

	while (1) {
		// 소켓 셋 초기화
		FD_ZERO(&ReadSet);
		FD_ZERO(&WriteSet);

		FD_SET(listen_sock, &ReadSet);	// ReadSet에 관찰 대상인 ListenSocket을 등록한다.
		
		for (int i = 0; i < nTotalSockets; i++) {
			ClientArray[i]->PutInReadOrWriteSet(ReadSet, WriteSet);
		}

		// select()
 		nready = select(0, &ReadSet, &WriteSet, NULL, NULL);
		if (nready == SOCKET_ERROR) err_quit("select()");
		
		// 소켓 셋 검사(1): 클라이언트 접속 수용
		if (FD_ISSET(listen_sock, &ReadSet)) {
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock,(struct sockaddr*)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET) {
				err_display("accept()");
				break;
			}
			else {
				// 클라이언트 정보 출력
				char addr[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
				printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
					addr, ntohs(clientaddr.sin_port));
				// 소켓 정보 추가: 실패 시 소켓 닫음
				if (!AddSocketInfo(client_sock))
					closesocket(client_sock);
			}
			if (--nready <= 0)
				continue;
		}

		// 소켓 셋 검사(2): 데이터 통신
		for (int i = 0; i < nTotalSockets; i++) {
			const CClient::SOCKETINFO* ptr = ClientArray[i]->GetSocketInfo();
			if (FD_ISSET(ptr->sock, &ReadSet)) {// 해당 소켓이 ReadSet에 있다면? 이 if문 입장.
				// 데이터 받기
				if(ClientArray[i]->RecvPacket()== false)
					RemoveSocketInfo(i);
			}
			else if (FD_ISSET(ptr->sock, &WriteSet)) {// 해당 소켓이 Write에 있다면? 이 if문 입장.
				if (ClientArray[i]->SendPacket() == false)
					RemoveSocketInfo(i);
			}
		} /* end of for */
	} /* end of while (1) */

	// 소켓 닫기
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 소켓 정보 추가
bool AddSocketInfo(SOCKET sock)
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
void RemoveSocketInfo(int nIndex)
{
	const CClient::SOCKETINFO* ptr = ClientArray[nIndex]->GetSocketInfo();

	MatchMakingMgr.RemoveClient(ClientArray[nIndex]);

	delete ClientArray[nIndex];       
	ClientArray[nIndex] = nullptr;

	if (nIndex != (nTotalSockets - 1))
		ClientArray[nIndex] = ClientArray[nTotalSockets - 1];
	--nTotalSockets;
}

