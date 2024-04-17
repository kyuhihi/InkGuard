#pragma once
#include "GameObject.h"
#include "MemoryPooler.h"

class CPlayer : public CGameObject 
{
public:
	CPlayer();
	~CPlayer();

public:
	void SetTransform(const C2S_PACKET_PLAYER_TRANSFORM& tPacket);
	const S2C_PACKET_PLAYER_TRANSFORM GetTransform() { return m_tTransform; };

	void SetInputs(const C2S_PACKET_PLAYER_INPUT& tPacket);
	const S2C_PACKET_PLAYER_INPUT GetInputs() { return m_tInputs; };



private:
	S2C_PACKET_PLAYER_TRANSFORM m_tTransform;
	S2C_PACKET_PLAYER_INPUT	m_tInputs; //몽타지 실행 시간도 여기 포함.

	CMemoryPooler* m_pMemoryPooler = nullptr;
	vector<pair<int,CMemoryPooler::MemoryBlock*>> m_AdditionalDataList;// first: 요청된 패킷사이즈 second 할당받은 메모리
};

