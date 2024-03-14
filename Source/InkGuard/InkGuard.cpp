// Copyright Epic Games, Inc. All Rights Reserved.

#include "InkGuard.h"
#include "Modules/ModuleManager.h"
#include "MyNetWorking/Include.h"


DEFINE_LOG_CATEGORY(InkGuardNetErr);

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, InkGuard, "InkGuard" );

#pragma region Network Err Print
void err_quit(const char* msg)// No Quit
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);// 메세지 박스가 뜰지 안뜰지 모르겠음.
	LocalFree(lpMsgBuf);
	//exit(1);
}

void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	//printf("[%s] %s\n", msg, (char*)lpMsgBuf);
	UE_LOG(InkGuardNetErr, Log, TEXT("[%s] %s"), msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	//printf("[오류] %s\n", (char*)lpMsgBuf);
	UE_LOG(InkGuardNetErr, Log, TEXT("[Error!!] %s"), (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

#pragma endregion