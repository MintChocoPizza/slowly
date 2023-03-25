#include <condition_variable>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <sys/_endian.h>
#include <sys/_pthread/_pthread_t.h>
#include <sys/_types/_size_t.h>
#include <sys/errno.h>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#ifdef LINUX
#include <sys/epoll.h>
#else
#include <sys/select.h>
#endif

#include <pthread.h>

#include <iostream>
#include <queue>
#include <string>
#include <semaphore>
#include <mutex>
#include <vector>

#define PORT 8080

void initSocket(int &iSockFd_sockFd);
void *connection_handler(void *);
std::string printMessage(int socket);

namespace ThreadPool
{
    class ThreadPool
    {
        public:
            ThreadPool(size_t num_threads);
            ~ThreadPool();

            // job을 추가한다.
    }
}
// 총 Worker 쓰레드의 개수
size_t num_threads_;
// Worker 쓰레드를 보관하는 벡터
std::vector<std::thread> worker_threads_;

// 할일들을 보관하는 job queue
// 이걸 원형 큐로 바꾸고 싶음
// 하지만 함수 포인터를 저장할 수 있는 컨테이너가 없음
std::queue<std::function<void()> > jobs_;
std::condition_variable cv_job_q_;          // 생성자-소비자 패턴을 구현
std::mutex m_job_q_;
bool stop_all;

ThrealPool::ThreadPool(size_t num_threads)
    :num_threads_(num_threads), stop_all(false)
{

}




int main(int argc, char const* argv[])
{
	int iSockFd_serverFd;
    int iSockFd_clntFd;
    int i_valRead;
	struct sockaddr_in SockAddrIn_sockAddr;
	int i_addrLen = sizeof(SockAddrIn_sockAddr);

    pthread_t pthread_id;

	const char* hello = "Connected server";
	char c_buffer[1024] = { 0 };

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
            if(pthread_create(&pthread_id, NULL, connection_handler, (void *)&iSockFd_clntFd) < 0)
            {
                fprintf(stderr, "pthread_create failed: [%02d] %s\n", errno, strerror(errno));
                exit(EXIT_FAILURE);
            }
            else 
            {
                fprintf(stderr, "pthread_create success: [%02d] %s\n", errno, strerror(errno));
                std::cout << "pthread_id: " << pthread_id << '\n';
            }
        }
    }

//	i_valRead = read(iSockFd_clntFd, c_buffer, 1024);
//	printf("%s\n", c_buffer);
//	send(iSockFd_clntFd, hello, strlen(hello), 0);
//	printf("Hello message sent\n");
//
//	close(iSockFd_clntFd);
//	shutdown(iSockFd_serverFd, SHUT_RDWR);
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

void *connection_handler(void *socket)
{
    // void* -> int * && *socket로 포인터가 가르키는 주소의 값 확인;
    int iSockFd_sockFd = *(int *)socket;

	const char* hello = "Connected server!!! so funny!!!";
    send(iSockFd_sockFd, hello, strlen(hello), 0);
    while(1)
    {
        std::string str = printMessage(iSockFd_sockFd);
        std::cout << str;
    }

    pthread_exit(NULL);
}

std::string printMessage(int socket)
{
    int iSockFd_sockFd = socket;
    int l;
    char r;
    std::string s = "";

read_again:
    l=read(iSockFd_sockFd, &r, 1);
    std::cerr << "[PrintMessage] read " << l << std::endl;
    if(l<0)
    {
        if(errno == EAGAIN)
        {
            goto read_done;
        }
        std::cerr << "[PrintMessage] IO Error on fd for Socket" << std::endl;
        return s;
    }
    if(l>0)
    {
        if (r != '~' && r !='\r' && r!= '\n' && r!='\0')
            s += r;
        else 
        {
            s += '\0';
            goto read_done;
        }
        goto read_again;
    }

read_done:
    return s;
}
