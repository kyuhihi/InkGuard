#include "Player.h"

void CPlayer::SetTransform(const C2S_PACKET_PLAYER_TRANSFORM& tPacket)
{
	m_tTransform.fYaw = tPacket.fYaw;
	m_tTransform.fSpeed = tPacket.fSpeed;
	m_tTransform.fVelocityZ = tPacket.fVelocityZ;

	m_tTransform.vPosition = tPacket.vPosition;
}

void CPlayer::SetInputs(const C2S_PACKET_PLAYER_INPUT& tPacket)
{
	m_tInputs.bInputs[PLAYER_INPUT::INPUT_F] = tPacket.bInputs[PLAYER_INPUT::INPUT_F];
	m_tInputs.bInputs[PLAYER_INPUT::INPUT_ATTACK] = tPacket.bInputs[PLAYER_INPUT::INPUT_ATTACK];
	m_tInputs.bInputs[PLAYER_INPUT::INPUT_VAULT] = tPacket.bInputs[PLAYER_INPUT::INPUT_VAULT];
	m_tInputs.bInputs[PLAYER_INPUT::INPUT_DODGE] = tPacket.bInputs[PLAYER_INPUT::INPUT_DODGE];
	m_tInputs.bInputs[PLAYER_INPUT::INPUT_SPRINT] = tPacket.bInputs[PLAYER_INPUT::INPUT_SPRINT];
}
