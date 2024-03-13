#include "Player.h"

void CPlayer::SetTransform(const C2S_PACKET_PLAYER_TRANSFORM& tPacket)
{
	m_tTransform.fYaw = tPacket.fYaw;
	m_tTransform.fSpeed = tPacket.fSpeed;
	m_tTransform.fVelocityZ = tPacket.fVelocityZ;

	m_tTransform.vPosition = tPacket.vPosition;
}
