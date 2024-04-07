#pragma once

// ���⼭ �����ϸ� �𸮾󿡼� �Ⱥ���.

struct SOLDIERINFO
{
	SOLDIER_TYPE eSoldierType = SOLDIER_END;
	TERRITORY_TYPE eTargetTerritory = TERRITORY_END;
	int iSoldierIndex;
};

#pragma pack(1)
struct SOCKETINFO
{
	SOCKET sock;
	bool bConnectSuccess = false;
};


//------------------------------------------------ ��Ŷ
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
	float fMontagePlayTime = 0.f;
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

#pragma pack(1)
struct S2C_PACKET_PLAYER_INPUT
{
	bool bInputs[PLAYER_INPUT::INPUT_END];
	float fMontagePlayTime = 0.f;
};

//------------------------------------------------
