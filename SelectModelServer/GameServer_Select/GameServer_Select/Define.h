#pragma once

#define SERVER_PORT		7777

#define SOLDIER_MAX_CNT 9
#define MAX_ADDITIONAL_SIZE 100 //추가로 오는 패킷은 최대 백개ㄲㅏ지받을수있다.
#define INIT_TIMER_CNT 5	//5개의 팀을 받을수있음..
#define TOTAL_GAME_TIME 5.f	//5개의 팀을 받을수있음..


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