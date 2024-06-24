#pragma once
#include "Network_Include.h"

class CMemoryPooler //�޸� ����ȭ�� ���� �Ź� new���Ҵ������ʰ� �޸� Ǯ���� ����Ѵ�.
{
#pragma region Declare Singleton
public:
	static CMemoryPooler* GetInstance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CMemoryPooler;	//�ν��Ͻ��� ���ٸ� �ϳ� �����մϴ�.

		return m_pInstance;
	}
	static void DestroyInstance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:// �̱��� Ŭ������ �����ڸ� private�� �����մϴ�.
	CMemoryPooler();
	~CMemoryPooler();

private:
	static CMemoryPooler* m_pInstance;
#pragma endregion

#pragma region MemoryStruct
public:
	struct MemoryBlock {
		char* pData = nullptr;
		size_t iDataSize = 0;
	};

#pragma endregion

public:
	MemoryBlock* Allocate(int iRequestDataSize);

	void DeAllocate(MemoryBlock* pReturnMemory) { m_FreeBlocks.push_back(pReturnMemory); };

private:
	size_t Resize2PowerOfTwo(size_t n);

private:
	list<MemoryBlock*> m_FreeBlocks; // ��� ������ �̻�� ��ϵ��� ���
};

