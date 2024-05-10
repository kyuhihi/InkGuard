#include "Packet.h"

CPacket::CPacket()
{
}

CPacket::CPacket(const CLIENT_STATE& eState, const int iRecvSoldierCnt)
{
	switch (eState)
	{
	case STATE_READY: {
		m_iBufferSize = sizeof(C2S_PACKET_GAMESTART);
		m_pBuf = new char[m_iBufferSize];
		break;
	}
	case STATE_TRANSFORM: {
		m_iBufferSize = sizeof(C2S_PACKET_PLAYER_TRANSFORM) + sizeof(C2S_PACKET_SOLDIER_TRANSFORM)* iRecvSoldierCnt;
		m_pBuf = new char[m_iBufferSize];
		break;
	}
	case STATE_INPUT: {
		m_iBufferSize = sizeof(C2S_PACKET_PLAYER_INPUT);
		m_pBuf = new char[m_iBufferSize];
		break;
	}
	case STATE_ADDITIONAL:				//����ų� ��Ŷ�̸� ��Ŷ�� ���۸� �Ҵ������ʾ�.
	case STATE_END:
	default:
		break;
	}
}

CPacket::~CPacket()
{
	m_iBufferSize = 0;

	delete[] m_pBuf;
	m_pBuf = nullptr;
}
