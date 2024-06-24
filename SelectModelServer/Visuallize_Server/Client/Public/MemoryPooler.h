#pragma once
#include "Network_Include.h"

class CMemoryPooler //메모리 단편화를 위해 매번 new로할당하지않고 메모리 풀링을 사용한다.
{
#pragma region Declare Singleton
public:
	static CMemoryPooler* GetInstance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CMemoryPooler;	//인스턴스가 없다면 하나 생성합니다.

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

private:// 싱글톤 클래스는 생성자를 private로 관리합니다.
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
	list<MemoryBlock*> m_FreeBlocks; // 사용 가능한 미사용 블록들의 목록
};

