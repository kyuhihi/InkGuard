#pragma once
// 여기서 선언하면 언리얼에선 안보임.

struct SOLDIERINFO
{
	SOLDIER_TYPE eSoldierType = SOLDIER_END;
	TERRITORY_TYPE eTargetTerritory = TERRITORY_END;
};

#pragma pack(1)
struct SOCKETINFO
{
	SOCKET sock;
	bool bConnectSuccess = false;
};


//------------------------------------------------ 패킷
#pragma pack(1)
struct C2S_PACKET_GAMESTART {
	char cSoldierInfo[SOLDIER_MAX_CNT]; 
	char cTargetTerritory[SOLDIER_MAX_CNT]; 
};

#pragma pack(1)
typedef struct C2S_PACKET_SOLDIER_TRANSFORM
{
	XMFLOAT3	vSoldier_Position = XMFLOAT3();
	float		fSoldier_Speed = 0.f;
	float		fSoldier_MontagePlayTime = 0.f;
	float		fSoldier_Yaw = 0.f;
	char		cHP = 100;
	char		cLastDamageCauser = -1;
} S2C_PACKET_SOLDIER_TRANSFORM;

#pragma pack(1)
struct C2S_PACKET_PLAYER_TRANSFORM
{
	short sPacketType = PACKET_TRANSFORM;
	bool bRecvTransform = false;

	XMFLOAT3 vPosition = XMFLOAT3();
	float fSpeed = 0.f;
	float fVelocityZ = 0.f;
	float fYaw = 0.f;
	float fHP = 100.f;
};

#pragma pack(1)
struct C2S_PACKET_ADDITIONAL_FLOAT3
{
	XMFLOAT3 vValue = XMFLOAT3();
};

#pragma pack(1)
struct C2S_PACKET_ADDITIONAL_FLOAT3x3
{
	XMFLOAT3 vValue1 = XMFLOAT3();
	XMFLOAT3 vValue2 = XMFLOAT3();
	XMFLOAT3 vValue3 = XMFLOAT3();
};

#pragma pack(1)
struct C2S_PACKET_PLAYER_INPUT
{
	char cInteractionType = INTERACTION_TYPE::INTERACTION_END;
	bool bInputs[PLAYER_INPUT::INPUT_END];
	float fMontagePlayTime = 0.f;
	C2S_PACKET_ADDITIONAL_FLOAT3x3 tAddPacket;
};

//------------------------------------------------
#pragma pack(1)
struct S2C_PACKET_GAMESTART {
	char cGamePlay = GAME_PLAY::GAME_END;			// 무슨색 팀인지에 대한 정보임.
	char cOtherSoldierInfo[SOLDIER_MAX_CNT];		// 다른 클라이언트의 팀 구성 정보임.
	char cOtherTargetTerritory[SOLDIER_MAX_CNT];	// 다른 클라이언트의 팀이 가져야할 타겟 점령지 정보임.
};

#pragma pack(1)
struct S2C_PACKET_PLAYER_TRANSFORM 
{
	XMFLOAT3 vPosition = XMFLOAT3();
	float fSpeed = 0.f;
	float fVelocityZ = 0.f;
	float fYaw = 0.f;
	float fHP = 100.f;
};

#pragma pack(1)
struct S2C_PACKET_PLAYER_INPUT
{
	char cInteractionType = INTERACTION_END;
	bool bInputs[PLAYER_INPUT::INPUT_END] = { false,false,false,false,false,false,false };
	float fMontagePlayTime = 0.f;
	C2S_PACKET_ADDITIONAL_FLOAT3x3 tAddPacket;
	float fGameTime = -1.f;
};

//------------------------------------------------ 