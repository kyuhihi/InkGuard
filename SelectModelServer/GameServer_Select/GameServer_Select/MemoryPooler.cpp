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
    // ��� ������ ��� �� ��û�� ũ�⺸�� ū ù ��° ����� ã��
    for (auto it = m_FreeBlocks.begin(); it != m_FreeBlocks.end(); ++it) {
        if ((*it)->iDataSize >= iNewBlockSize) {
            // ��û�� ũ�⺸�� ū ����� ã�����Ƿ� ��ȯ
            MemoryBlock* pReturnBlock = (*it);
            m_FreeBlocks.erase(it);
            return pReturnBlock;
        }
    }

    // ��û�� ũ�⺸�� ū ����� ���� ��� ���ο� �޸� �Ҵ�
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
    while (powerOfTwo < n) {//��û�� ũ�⸦ 2�ǹ���� ���·� �� ������ üũ�Ѵ�.
        powerOfTwo *= 2;
    }
    return powerOfTwo;

}
