#pragma once
#include "GameObject.h"

class CPlayer : public CGameObject 
{
public:
	void SetTransform(const C2S_PACKET_PLAYER_TRANSFORM& tPacket);
	const S2C_PACKET_PLAYER_TRANSFORM GetTransform() { return m_tTransform; };

private:
	S2C_PACKET_PLAYER_TRANSFORM m_tTransform;
};

