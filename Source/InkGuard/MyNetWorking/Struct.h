#pragma once

// 여기서 선언하면 언리얼에선 안보임.
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
struct S2C_PACKET_PLAYER_TRANSFORM //게임 플레이 정보도 같이 보내준다.
{
	char cGamePlay = GAME_PLAY::GAME_END;

	XMFLOAT3 vPosition = XMFLOAT3();
	float fSpeed = 0.f;
	float fVelocityZ = 0.f;
	float fYaw = 0.f;
};

//------------------------------------------------