#include "socks.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

SOCKET sock = INVALID_SOCKET;

int create_socket()
{
	struct sockaddr_in server_addr;
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

	server_addr.sin_family = AF_INET;
	memcpy(&server_addr.sin_addr.s_addr, &ip_addr, sizeof(struct in_addr));
	server_addr.sin_port = htons(DEFAULT_PORT);
	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	ret = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (ret == SOCKET_ERROR) {
		printf("Connect Error - %x\n",
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

	strncpy(sendbuf, "Hello server!", sizeof(sendbuf) - 1);

	ret = send(sock, sendbuf, strlen(sendbuf) + 1, 0);
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

	ret = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
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

	printf("client recv: %s \n", recvbuf);

quit:
	if (sock > 0)
		closesocket(sock);

#ifdef _MSC_VER
	WSACleanup();
#endif // _MSC_VER

	return ret;
}
