#pragma once
#include "Include.h"

class CPacket// for RAII pattern
{// 아직 Recv할때 밖에안씀.
public:
	CPacket();
	CPacket(const CLIENT_STATE& eState);
	~CPacket();

public:
	char* m_pBuf = nullptr;
	int m_iBufferSize = 0;
};

