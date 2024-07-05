#pragma once
#include "Common.h"
#include "Define.h"
#include "Enum.h"


//	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!������!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//	Ŭ���ʿ��� struct �߰� �ߴٰ�  ctrl a�ؼ� �ٿ��ֱ�� ���Ұ�. 
//	�Ʒ� ������ struct ������ client������ �����Ҽ� ����..

//	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!������!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//------------------------------------------------
#pragma pack(1)
struct C2S_PACKET_GAMESTART {
	char cSoldierInfo[SOLDIER_MAX_CNT]; // �ٸ� Ŭ���̾�Ʈ�� �� ���� ������.
	char cTargetTerritory[SOLDIER_MAX_CNT]; // �ٸ� Ŭ���̾�Ʈ�� ���� �������� Ÿ�� ������ ������.
};

#pragma pack(1)
typedef struct C2S_PACKET_SOLDIER_TRANSFORM
{
	XMFLOAT3	vSoldier_Position = XMFLOAT3();
	float		fSoldier_Speed = 0.f;
	float		fSoldier_MontagePlayTime = 0.f;
	float		fSoldier_Yaw = 0.f;
	float		fHP = 1.f;
}S2C_PACKET_SOLDIER_TRANSFORM;

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
struct C2S_PACKET_PLAYER_INPUT
{
	short sPacketType = PACKET_INPUT;
	bool bInputs[PLAYER_INPUT::INPUT_END];
	float fMontagePlayTime = 0.f;
	short sAdditionalPacketSize = 0;
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

//------------------------------------------------
#pragma pack(1)
struct S2C_PACKET_GAMESTART {
	char cGamePlay = GAME_PLAY::GAME_END;			// ������ �������� ���� ������.
	char cOtherSoldierInfo[SOLDIER_MAX_CNT];		// �ٸ� Ŭ���̾�Ʈ�� �� ���� ������.
	char cOtherTargetTerritory[SOLDIER_MAX_CNT];	// �ٸ� Ŭ���̾�Ʈ�� ���� �������� Ÿ�� ������ ������.
};

#pragma pack(1)
struct S2C_PACKET_PLAYER_TRANSFORM {
	//char cGamePlay = GAME_PLAY::GAME_END; // 0�̸� ��Ų����.

	XMFLOAT3 vPosition = XMFLOAT3();
	float fSpeed = 0.f;
	float fVelocityZ = 0.f;
	float fYaw = 0.f;
	float fHP = 100.f;
};

#pragma pack(1)
struct S2C_PACKET_PLAYER_INPUT
{
	bool bInputs[PLAYER_INPUT::INPUT_END] = { false,false,false,false,false,false,false,false };
	float fMontagePlayTime = 0.f;
	short sAdditionalPacketSize = 0;
};

//------------------------------------------------