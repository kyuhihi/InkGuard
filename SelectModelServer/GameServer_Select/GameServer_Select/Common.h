#pragma once

#define _CRT_SECURE_NO_WARNINGS				// ���� C �Լ� ��� �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS		// ���� ���� API ��� �� ��� ����

#include <winsock2.h> // ����2 ���� ���
#include <ws2tcpip.h> // ����2 Ȯ�� ���
#include <stdio.h>

#pragma comment(lib, "ws2_32") // ws2_32.lib ��ũ

//Link2005 ���������� ����� cpp�и�.
void err_quit(const char* msg);
void err_display(const char* msg);
void err_display(int errcode);