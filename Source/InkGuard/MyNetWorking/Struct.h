#pragma once

// ���⼭ �����ϸ� �𸮾󿡼� �Ⱥ���.
#pragma pack(1)
struct SOCKETINFO
{
	SOCKET sock;
	bool bConnectSuccess = true;
};

//------------------------------------------------
#pragma pack(1)
struct C2S_PACKET_PLAYER_TRANSFORM
{
	short sPacketType = PACKET_TRANSFORM;
	bool bRecvTransform = false;

	XMFLOAT3 vPosition = XMFLOAT3();
	float fSpeed = 0.f;
	float fVelocityZ = 0.f;
	float fYaw = 0.f;
};

//------------------------------------------------

#pragma pack(1)
struct S2C_PACKET_PLAYER_TRANSFORM //���� �÷��� ������ ���� �����ش�.
{
	char cGamePlay = GAME_PLAY::GAME_END;

	XMFLOAT3 vPosition = XMFLOAT3();
	float fSpeed = 0.f;
	float fVelocityZ = 0.f;
	float fYaw = 0.f;
};

//------------------------------------------------