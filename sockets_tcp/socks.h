#ifndef _SOCKS_H_
#define _SOCKS_H_

#ifdef _MSC_VER
#include <WinSock2.h>
#include <winioctl.h>
#include <windows.h>
#include <windowsx.h>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")
#else
/* GCC*/
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
typedef int SOCKET;
#define closesocket(x) close(x)
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#endif

#ifdef _MSC_VER
struct in_addr ip_addr = { { { 127, 0, 0, 1 } } };
#else
struct in_addr ip_addr = { 0x0100007F };
#endif

#define DEFAULT_PORT 7788

#endif
