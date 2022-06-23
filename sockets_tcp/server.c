#include "socks.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

SOCKET sock = INVALID_SOCKET;

int create_socket()
{
	struct sockaddr_in local_addr;
	int ret;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		printf("Create socket Failed - %x\n",
#ifdef _MSC_VER
			WSAGetLastError()
#else
			errno
#endif
		);
		return -1;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
		printf("Cannot configure server listen socket.  Error is 0x%x\n",
#ifdef _MSC_VER
			WSAGetLastError()
#else
			errno
#endif
		);
		return -1;
	}

	memset(&local_addr, 0, sizeof(struct in_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(DEFAULT_PORT);
	//memcpy(&local_addr.sin_addr.s_addr, &ip_addr, sizeof(struct in_addr));
	local_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr));
	if (ret == SOCKET_ERROR) {
		printf("bind Error - %x\n",
#ifdef _MSC_VER
			WSAGetLastError()
#else
			errno
#endif
		);
		return -1;
	}

	ret = listen(sock, 10);
	if (ret == SOCKET_ERROR) {
		printf("listen Error - %x\n",
#ifdef _MSC_VER
			WSAGetLastError()
#else
			errno
#endif
		);
		return -1;
	}

	return 0;
}

int main(void)
{
	int ret = 0;
	SOCKET serv_sock = INVALID_SOCKET;
	struct sockaddr_in serv_addr;
	uint32_t length;

	char sendbuf[64];
	char recvbuf[64];
	/* Initialize Winsock*/
#ifdef _MSC_VER
	WSADATA ws;
	ret = WSAStartup(MAKEWORD(2, 2), &ws);
	if (ret != 0) {
		printf("WSAStartup failed with error: %d\n", ret);
		return -1;
	}
#endif

	ret = create_socket();
	if (ret < 0) {
		printf("create_socket fail, ret = %d \n", ret);
		goto quit;
	}

	length = sizeof(serv_addr);
	serv_sock = accept(sock, (struct sockaddr*)&serv_addr, &length);
	if (serv_sock == SOCKET_ERROR) {
		printf("accept Error - %x\n",
#ifdef _MSC_VER
			WSAGetLastError()
#else
			errno
#endif
		);
		goto quit;
	}

	ret = recv(serv_sock, recvbuf, sizeof(recvbuf) - 1, 0);
	if (ret < 0) {
		printf("recv Error - %x\n",
#ifdef _MSC_VER
			WSAGetLastError()
#else
			errno
#endif
		);
		goto quit;
	}

	printf("server recv: %s \n", recvbuf);

	strncpy(sendbuf, "Hello client!", sizeof(sendbuf) - 1);
	ret = send(serv_sock, sendbuf, strlen(sendbuf) + 1, 0);
	if (ret < 0) {
		printf("send Error - %x\n",
#ifdef _MSC_VER
			WSAGetLastError()
#else
			errno
#endif
		);
		goto quit;
	}

quit:
	if (serv_sock > 0)
		closesocket(serv_sock);

	if (sock > 0)
		closesocket(sock);


#ifdef _MSC_VER
	WSACleanup();
#endif // _MSC_VER

	return 0;
}
