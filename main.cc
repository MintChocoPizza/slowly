#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/_endian.h>
#include <sys/_pthread/_pthread_t.h>
#include <sys/errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#ifdef LINUX
#include <sys/epoll.h>
#endif

#include <pthread.h>

#include <iostream>
#include <string>
#include <semaphore.h>
#include <mutex>

#define PORT 8080

void initSocket(int &iSockFd_sockFd);
void *sendMessage_handler(void *);

int main(int argc, char const* argv[])
{
	int iSockFd_serverFd;
    int iSockFd_clntFd;
	struct sockaddr_in SockAddrIn_sockAddr;
	int i_addrLen = sizeof(SockAddrIn_sockAddr);
    pthread_t pthread_id;
    char c_clientIP[INET_ADDRSTRLEN];

    initSocket(iSockFd_serverFd);

    while(true)
    {
        if ((iSockFd_clntFd = accept(iSockFd_serverFd, (struct sockaddr*)&SockAddrIn_sockAddr, (socklen_t*)&i_addrLen)) < 0) 
        {
            fprintf(stderr, "accept failed: [%02d] %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
        else 
        {
            fprintf(stdout, "accept success: [%02d] %s\n", errno, strerror(errno));
            if(pthread_create(&pthread_id, NULL, sendMessage_handler, (void *)&iSockFd_clntFd) < 0)
            {
                fprintf(stderr, "pthread_create failed: [%02d] %s\n", errno, strerror(errno));
                exit(EXIT_FAILURE);
            }
            else 
            {
                fprintf(stderr, "pthread_create success: [%02d] %s\n", errno, strerror(errno));
                std::cout << "pthread_id: " << pthread_id << '\n';
                inet_ntop(AF_INET, &(SockAddrIn_sockAddr.sin_addr), c_clientIP, INET_ADDRSTRLEN);
                printf("Connected client IP address: %s\n", c_clientIP);
            }
        }
    }

	close(iSockFd_clntFd);
	shutdown(iSockFd_serverFd, SHUT_RDWR);
	return 0;
}

void initSocket(int &iSockFd_sockFd)
{
    int i_opt=1;
    struct sockaddr_in SockAddrIn_sockAddr;

    // set socket
    if ((iSockFd_sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "initSocket() socket() failed: [%02d] %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // set socket option
    if (setsockopt(iSockFd_sockFd, SOL_SOCKET, SO_REUSEADDR, &i_opt, sizeof(i_opt)))
    {
        fprintf(stderr, "initSocket() setsockopt() failed: [%02d] %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    SockAddrIn_sockAddr.sin_family = AF_INET;
    SockAddrIn_sockAddr.sin_addr.s_addr = INADDR_ANY;
    SockAddrIn_sockAddr.sin_port = htons(PORT);
    // bind
    if (bind(iSockFd_sockFd, (struct sockaddr*)&SockAddrIn_sockAddr, sizeof(SockAddrIn_sockAddr)) < 0)
    {
        fprintf(stderr, "initSocket() bind() failed: [%02d] %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // listen
    if(listen(iSockFd_sockFd, 3) < 0 )
    {
        fprintf(stderr, "initSocket() listen() failed: [%02d] %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

}

void *sendMessage_handler(void *socket)
{
    // void* -> int * && *socket로 포인터가 가르키는 주소의 값 확인;
    int iSockFd_sockFd = *(int *)socket;
    char r[1024];

	const char* hello = "Connected server!!! so funny!!!";
    send(iSockFd_sockFd, hello, strlen(hello), 0);
    while(1)
    {
        memset(r,0, 1024);
        int l = read(iSockFd_sockFd, r, 1024);
        printf("%s", r);
    }

    return NULL;
}
