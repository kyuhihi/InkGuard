#pragma once

// 여기서 선언하면 언리얼에선 안보임.
struct SOCKETINFO
{
	SOCKET sock;
	bool bConnectSuccess = true;
};

//------------------------------------------------

struct C2S_PACKET_PLAYER_TRANSFORM
{
	short sPacketType = PACKET_TRANSFORM;
	bool bRecvTransform = false;

	XMFLOAT3 vPosition = XMFLOAT3();
	XMFLOAT3 vVelocity = XMFLOAT3();
	float fYaw = 0.f;
};

//------------------------------------------------

struct S2C_PACKET_PLAYER_TRANSFORM
{
	XMFLOAT3 vPosition = XMFLOAT3();
	XMFLOAT3 vVelocity = XMFLOAT3();
	float fYaw = 0.f;
};

//------------------------------------------------