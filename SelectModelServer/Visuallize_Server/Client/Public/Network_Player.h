#pragma once
#include "Network_GameObject.h"
#include "MemoryPooler.h"
#include "Network_Include.h"

class CClient;

class CNetwork_Player : public CNetwork_GameObject 
{
public:
	CNetwork_Player(CClient* pClient);
	~CNetwork_Player();

public:
	void SetTransform(const C2S_PACKET_PLAYER_TRANSFORM& tPacket);
	const S2C_PACKET_PLAYER_TRANSFORM GetTransform() { return m_tTransform; };

	void SetInputs(const C2S_PACKET_PLAYER_INPUT& tPacket);
	const S2C_PACKET_PLAYER_INPUT GetInputs(); 

	const bool IsAnyAdditionalData() { return !m_AdditionalDataList.empty(); }
	pair<int, CMemoryPooler::MemoryBlock*>& GetLastDataBlock(); // ������ �޸𸮺��� ���� recv�ϱ����� ���۷����� return ���ش�.
	
	const int& GetRemAdditionalSize() const { return m_iRemAdditionalSize; }
	void CalculateSendAdditionalPacket (char*& pOtherClientSendBuf, int& iOtherSendBufferSize);
	void ClearUsedData();

private:
	S2C_PACKET_PLAYER_TRANSFORM m_tTransform;
	S2C_PACKET_PLAYER_INPUT	m_tInputs; //��Ÿ�� ���� �ð��� ���� ����.

	CMemoryPooler* m_pMemoryPooler = nullptr;
	list<pair<int,CMemoryPooler::MemoryBlock*>> m_AdditionalDataList;// first: ��û�� ��Ŷ������ second �Ҵ���� �޸� ��.
	list<CMemoryPooler::MemoryBlock*> m_UsedData;

	int m_iRemAdditionalSize = 0;// Ŭ���̾�Ʈ�ʿ��� input���� ��������ŭ ��ٸ��������Ŵϱ� input��Ŷ�� ������ get �Ҷ� Ȯ���� ����� �����صк���.

	list<string>m_StringTable;

	class CClient* m_pOwner = nullptr;
};

