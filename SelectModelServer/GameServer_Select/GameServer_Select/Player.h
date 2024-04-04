#pragma once
#include "GameObject.h"

class CPlayer : public CGameObject 
{
public:
	void SetTransform(const C2S_PACKET_PLAYER_TRANSFORM& tPacket);
	const S2C_PACKET_PLAYER_TRANSFORM GetTransform() { return m_tTransform; };

	void SetInputs(const C2S_PACKET_PLAYER_INPUT& tPacket);
	const S2C_PACKET_PLAYER_INPUT GetInputs() { return m_tInputs; };

private:
	S2C_PACKET_PLAYER_TRANSFORM m_tTransform;
	S2C_PACKET_PLAYER_INPUT	m_tInputs; //몽타지 실행 시간도 여기 포함.
};

