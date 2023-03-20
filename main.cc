#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#ifdef LINUX
#include <sys/epoll.h>
#endif

#include <pthread.h>

#include <semaphore.h>
#include <mutex>

#define PORT 8080
int main(int argc, char const* argv[])
{
	int iSockFd_serverFd;
    int iSockFd_clntFd;
    int i_valRead;
	struct sockaddr_in SockAddrIn_sockAddr;
	int i_opt = 1;
	int i_addrLen = sizeof(SockAddrIn_sockAddr);
	char c_buffer[1024] = { 0 };
	const char* hello = "Hello from server";

	if ((iSockFd_serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        fprintf(stderr, "socket failed: [%02d] %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

	if (setsockopt(iSockFd_serverFd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &i_opt, sizeof(i_opt))) 
    {
        fprintf(stderr, "setsockopt failed: [%02d] %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
	}
	SockAddrIn_sockAddr.sin_family = AF_INET;
	SockAddrIn_sockAddr.sin_addr.s_addr = INADDR_ANY;
	SockAddrIn_sockAddr.sin_port = htons(PORT);

	if (bind(iSockFd_serverFd, (struct sockaddr*)&SockAddrIn_sockAddr, sizeof(SockAddrIn_sockAddr)) < 0) 
    {
        fprintf(stderr, "bind failed: [%02d] %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
	}
	if (listen(iSockFd_serverFd, 3) < 0) 
    {
        fprintf(stderr, "listen failed: [%02d] %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
	}
	if ((iSockFd_clntFd = accept(iSockFd_serverFd, (struct sockaddr*)&SockAddrIn_sockAddr, (socklen_t*)&i_addrLen)) < 0) 
    {
        fprintf(stderr, "accept failed: [%02d] %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
	}


	i_valRead = read(iSockFd_clntFd, c_buffer, 1024);
	printf("%s\n", c_buffer);
	send(iSockFd_clntFd, hello, strlen(hello), 0);
	printf("Hello message sent\n");

	close(iSockFd_clntFd);
	shutdown(iSockFd_serverFd, SHUT_RDWR);
	return 0;
}
