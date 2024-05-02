#include "MemoryPooler.h"

CMemoryPooler* CMemoryPooler::m_pInstance = nullptr;

CMemoryPooler::CMemoryPooler()
{
}

CMemoryPooler::~CMemoryPooler()
{
	for (auto& Memory : m_FreeBlocks)
	{
		delete[] Memory->pData;
        Memory = nullptr;
	}
	m_FreeBlocks.clear();
}

CMemoryPooler::MemoryBlock* CMemoryPooler::Allocate(int iRequestDataSize)
{
    size_t iNewBlockSize = Resize2PowerOfTwo(iRequestDataSize);
    // 사용 가능한 블록 중 요청된 크기보다 큰 첫 번째 블록을 찾음
    for (auto it = m_FreeBlocks.begin(); it != m_FreeBlocks.end(); ++it) {
        if ((*it)->iDataSize >= iNewBlockSize) {
            // 요청된 크기보다 큰 블록을 찾았으므로 반환
            MemoryBlock* pReturnBlock = (*it);
            m_FreeBlocks.erase(it);
            return pReturnBlock;
        }
    }

    // 요청된 크기보다 큰 블록이 없는 경우 새로운 메모리 할당
    MemoryBlock* pNewBlock = new MemoryBlock;
    pNewBlock->pData = new char[iNewBlockSize];
    pNewBlock->iDataSize = iNewBlockSize;

    return pNewBlock;
}

size_t CMemoryPooler::Resize2PowerOfTwo(size_t n)
{
    if (n == 0) 
        return 1;

    size_t powerOfTwo = 1;
    while (powerOfTwo < n) {//요청한 크기를 2의배수의 형태로 더 작은지 체크한다.
        powerOfTwo *= 2;
    }
    return powerOfTwo;

}
