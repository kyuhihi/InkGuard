#pragma once
#include "Common.h"
#include "Define.h"
#include "Enum.h"

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

#pragma pack(1)
struct C2S_PACKET_PLAYER_INPUT
{
	short sPacketType = PACKET_INPUT;
	bool bInputs[PLAYER_INPUT::INPUT_END];
};

//------------------------------------------------
#pragma pack(1)
struct S2C_PACKET_PLAYER_TRANSFORM {
	char cGamePlay = GAME_PLAY::GAME_END; // 0이면 통신끊긴거.

	XMFLOAT3 vPosition = XMFLOAT3();
	float fSpeed = 0.f;
	float fVelocityZ = 0.f;
	float fYaw = 0.f;
};

#pragma pack(1)
struct S2C_PACKET_PLAYER_INPUT
{
	bool bInputs[PLAYER_INPUT::INPUT_END];
};

//------------------------------------------------