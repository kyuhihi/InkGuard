#pragma once
#include "Include.h"

class CPacket// for RAII pattern
{// ���� Recv�Ҷ� �ۿ��Ⱦ�.
public:
	CPacket();
	CPacket(const CLIENT_STATE& eState);
	~CPacket();

public:
	char* m_pBuf = nullptr;
	int m_iBufferSize = 0;
};

