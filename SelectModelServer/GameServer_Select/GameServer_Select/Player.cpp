#include "Player.h"
#include "Client.h"

CPlayer::CPlayer(CClient* pClient)
{
	m_pMemoryPooler = CMemoryPooler::GetInstance();
	m_pOwner = pClient;
}

CPlayer::~CPlayer()
{
	ClearUsedData();
	for (auto Memory : m_AdditionalDataList)
		m_pMemoryPooler->DeAllocate(Memory.second);
	m_AdditionalDataList.clear();
	m_pOwner = nullptr;
}

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
	m_tInputs.bInputs[PLAYER_INPUT::INPUT_CROUCH] = tPacket.bInputs[PLAYER_INPUT::INPUT_CROUCH];
	m_tInputs.bInputs[PLAYER_INPUT::INPUT_CLIMBING] = tPacket.bInputs[PLAYER_INPUT::INPUT_CLIMBING];

	m_tInputs.fMontagePlayTime = tPacket.fMontagePlayTime;

	if (tPacket.sAdditionalPacketSize == 0)
		return;

	if (m_pOwner->GetSocketInfo()->eGamePlayTeam == GAME_BLUE_TEAM)
	{
		int i = 0;
	}

	m_AdditionalDataList.push_back(make_pair(tPacket.sAdditionalPacketSize, m_pMemoryPooler->Allocate(tPacket.sAdditionalPacketSize))); 

	m_tInputs.sAdditionalPacketSize = 0;
	for (auto& iter : m_AdditionalDataList)
	{
		m_tInputs.sAdditionalPacketSize +=	iter.first; // 패킷 총 사이즈 계산해둬!
	}
//	m_StringTable.push_back(to_string(m_tInputs.sAdditionalPacketSize));

}

const S2C_PACKET_PLAYER_INPUT CPlayer::GetInputs()
{
	S2C_PACKET_PLAYER_INPUT newInput = m_tInputs;
	newInput.sAdditionalPacketSize = m_iRemAdditionalSize;

	return newInput;
}

pair<int, CMemoryPooler::MemoryBlock*>& CPlayer::GetLastDataBlock()
{
	if (!m_AdditionalDataList.empty()) {
		m_iRemAdditionalSize += m_AdditionalDataList.back().first;
		return m_AdditionalDataList.back();
	}
	else {
		// 만약 리스트가 비어있다면, 적절한 예외 처리를 수행하거나 null을 반환할 수 있습니다.
		throw std::out_of_range("List is empty");
	}
}

void CPlayer::CalculateSendAdditionalPacket(char*& pOtherClientSendBuf, int& iOtherSendBufferSize)
{
	if (m_AdditionalDataList.empty())
		return;
	if (m_iRemAdditionalSize == 0)
	{
		return;
	}
	cout << "Other AditionalSize : " << m_iRemAdditionalSize << endl;
	iOtherSendBufferSize = m_iRemAdditionalSize;
	pOtherClientSendBuf = new char[m_iRemAdditionalSize];

	int iCheckBufferSize = 0;
	int iOffset = 0;
	auto iter = m_AdditionalDataList.begin();
	while ( iter != m_AdditionalDataList.end() && iCheckBufferSize != m_iRemAdditionalSize)
	{
		int dataSizeToSendThisWhile = iter->first;
		memcpy(pOtherClientSendBuf + iOffset, iter->second->pData, dataSizeToSendThisWhile);

		iOffset += dataSizeToSendThisWhile;

		iCheckBufferSize += dataSizeToSendThisWhile;

		CMemoryPooler::MemoryBlock* blockToDelete =iter->second;
		iter = m_AdditionalDataList.erase(iter);

		m_UsedData.push_back(blockToDelete);
	}


}

void CPlayer::ClearUsedData()
{
	m_iRemAdditionalSize = 0;
	
	m_StringTable.push_back(to_string(m_iRemAdditionalSize));
	if (m_UsedData.empty())
		return;

	for (auto& iter : m_UsedData)
	{
		m_pMemoryPooler->DeAllocate(iter);
	}
	m_UsedData.clear();
}


