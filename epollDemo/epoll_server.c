#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>    
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/resource.h>
 
#define MAXEPOLL 10000
#define MAXLINE  1024
#define PORT     6000
#define MAXBACK  1000
 
/* set NONBLOCK */
int setNonBlock(int fd)
{
    int flag = fcntl(fd, F_GETFD, 0 );
    if(fcntl(fd, F_SETFL,  flag | O_NONBLOCK ) == -1)
    {
        printf("Set blocking error : %d\n", errno);
        return -1;
    }
    return 0;
}
 
int main(int argc, char * argv[])
{
    int         listen_fd = -1;
    int         conn_fd;
    int         epoll_fd;
    int         nread;
    int         cur_fds;
    int         wait_fds;
    int        i;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    struct     epoll_event    ev;
    struct     epoll_event    evs[MAXEPOLL];
    /* 最大连接数量 */
    struct     rlimit    rlt;
    char     buf[MAXLINE];
    socklen_t len = sizeof(struct sockaddr_in);
 
    rlt.rlim_max = rlt.rlim_cur = MAXEPOLL;
    if(setrlimit(RLIMIT_NOFILE, &rlt) == -1)    
    {
        printf("Setrlimit Error : %d\n", errno);
        exit(EXIT_FAILURE);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    
    if( (listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error...\n" , errno );
        exit(EXIT_FAILURE);
    }
    
    //设置非阻塞模式
    if(setNonBlock(listen_fd) < 0)
    {
        printf("Setnonblocking Error : %d\n", errno);
        exit(EXIT_FAILURE);
    }
    
    if(bind(listen_fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0)
    {
        printf("Bind Error : %d\n", errno);
        exit(EXIT_FAILURE);
    }
 
    if(listen(listen_fd, MAXBACK) < 0)
    {
        printf("Listen Error : %d\n", errno);
        exit(EXIT_FAILURE);
    }
    
    // 创建epoll
    epoll_fd = epoll_create(MAXEPOLL);    //!> create
    ev.events = EPOLLIN | EPOLLET;        //!> accept Read!
    ev.data.fd = listen_fd;               //!> 将listen_fd 加入
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) < 0)
    {
        printf("Epoll Error : %d\n", errno);
        exit(EXIT_FAILURE);
    }
    cur_fds = 1;
    
    while(1)
    {
        if((wait_fds = epoll_wait(epoll_fd, evs, cur_fds, -1)) < 0)
        {
            printf( "Epoll Wait Error : %d\n", errno );
            exit( EXIT_FAILURE );
        }
 
        for(i = 0; i < wait_fds; i++)
        {
            if(evs[i].data.fd == listen_fd && cur_fds < MAXEPOLL)
            {
                if((conn_fd = accept(listen_fd, (struct sockaddr *)&cliaddr, &len)) < 0)
                {
                    printf("Accept Error : %d\n", errno);
                    exit( EXIT_FAILURE );
                }
                
                printf("Server get from client  %s, %d!\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
                
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_fd;
                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev) < 0)
                {
                    printf("Epoll Error : %d\n", errno);
                    exit( EXIT_FAILURE );
                }
                ++cur_fds; 
                continue;        
            }
            
            nread = read(evs[i].data.fd, buf, sizeof(buf));
            if(nread <= 0)
            {
                close(evs[i].data.fd);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, evs[i].data.fd, &ev);
                --cur_fds;
                continue;
            }
            
            write(evs[i].data.fd, buf, nread);
        }
    }
    
    close(listen_fd);
    return 0;
}
