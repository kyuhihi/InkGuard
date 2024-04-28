#pragma once
#include "GameObject.h"
#include "MemoryPooler.h"
#include "Include.h"

class CClient;

class CPlayer : public CGameObject 
{
public:
	CPlayer(CClient* pClient);
	~CPlayer();

public:
	void SetTransform(const C2S_PACKET_PLAYER_TRANSFORM& tPacket);
	const S2C_PACKET_PLAYER_TRANSFORM GetTransform() { return m_tTransform; };

	void SetInputs(const C2S_PACKET_PLAYER_INPUT& tPacket);
	const S2C_PACKET_PLAYER_INPUT GetInputs(); 

	const bool IsAnyAdditionalData() { return !m_AdditionalDataList.empty(); }
	pair<int, CMemoryPooler::MemoryBlock*>& GetLastDataBlock(); // 마지막 메모리블럭에 값을 recv하기위해 레퍼런스로 return 해준다.
	
	const int& GetRemAdditionalSize() const { return m_iRemAdditionalSize; }
	void CalculateSendAdditionalPacket (char*& pOtherClientSendBuf, int& iOtherSendBufferSize);
	void ClearUsedData();

private:
	S2C_PACKET_PLAYER_TRANSFORM m_tTransform;
	S2C_PACKET_PLAYER_INPUT	m_tInputs; //몽타지 실행 시간도 여기 포함.

	CMemoryPooler* m_pMemoryPooler = nullptr;
	list<pair<int,CMemoryPooler::MemoryBlock*>> m_AdditionalDataList;// first: 요청된 패킷사이즈 second 할당받은 메모리 블럭.
	list<CMemoryPooler::MemoryBlock*> m_UsedData;

	int m_iRemAdditionalSize = 0;// 클라이언트쪽에서 input에서 가져간만큼 기다리고있을거니까 input패킷을 받으러 get 할때 확정된 사이즈를 저장해둔변수.

	list<string>m_StringTable;

	class CClient* m_pOwner = nullptr;
};

