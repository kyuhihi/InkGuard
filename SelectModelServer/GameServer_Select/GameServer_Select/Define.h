#pragma once

#define SERVER_PORT		7777

#define SOLDIER_MAX_CNT 9
#define MAX_ADDITIONAL_SIZE 100 //�߰��� ���� ��Ŷ�� �ִ� �鰳�������������ִ�.
#define INIT_TIMER_CNT 5	//5���� ���� ����������..
#define TOTAL_GAME_TIME 5.f	//5���� ���� ����������..


#define _float3	XMFLOAT3
#define _float4 XMFLOAT4

#define _vector XMVECTOR
#define _fVector FXMVECTOR

#define _matrix XMMATRIX
#define _fmatrix FXMMATRIX

#define _float4x4 XMFLOAT4X4

using _tchar = wchar_t;
using _float = float;


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG