#pragma once
#include "Common.h"
#include "Define.h"

struct SOCKETINFO
{
	SOCKET sock;
	char buf[BUFFER_SIZE + 1];
	int recvbytes;
	int sendbytes;
};