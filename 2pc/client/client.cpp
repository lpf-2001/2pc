#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<errno.h>
#include<iostream>
#include <unistd.h>

const int Port = 8008;

struct params{
    int sock_fd;
    int exit_flag;
};

void *func_listen(void *arg);

void do_thing(int sock_fd);

int main(void){
    int sock_fd;
    char buf[1024], sendbuf[1024], recvbuf[1024];
    struct sockaddr_in server_addr;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);//初始化socket
    if(sock_fd == -1){
        printf("%s\n",strerror(errno));
        return 0;
    }
 
    bzero(&server_addr, sizeof(server_addr));//编辑服务端地址信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 
    int tmp = connect(sock_fd, (struct sockaddr *)(&server_addr),
                      sizeof(struct sockaddr));//连接服务端socket
    if(tmp == -1){
        printf("%s\n",strerror(errno));
        return 0;
    }

    //创建线程监听服务器的消息
    pthread_t pt;
    struct params para;
    para.sock_fd=sock_fd;
    para.exit_flag=0;
    pthread_create(&pt, NULL, func_listen,&para);

    //接收用户输入命令
    while(1){
        fgets(sendbuf, sizeof(sendbuf), stdin);
        //send(sock_fd, sendbuf, strlen(sendbuf), 0);
        if(strcmp(sendbuf, "exit\n") == 0)
        {
            para.exit_flag=1;
            break;
        }
            
        //recv(sock_fd, recvbuf, sizeof(recvbuf), 0);
        //fputs(recvbuf, stdout);
        memset(sendbuf, 0, sizeof(sendbuf));
        
    }
    close(sock_fd);
    return 0;
}


void *func_listen(void *arg)
{
    char recvbuf[1024];
    
    struct params *para=(struct params *)arg;
    int sock_fd=para->sock_fd;
    while(para->exit_flag==0)
    {
        std::cout<<"recving message.....\n";
        if(recv(sock_fd, recvbuf, sizeof(recvbuf), 0))
        {   
            //如果收到开始投票信息就准备开始进行投票 
            if(strcmp("vote",recvbuf)==0)
            {
                std::cout<<"got it\n";
                send(sock_fd, "get", strlen("get"), 0);
                //开始做事
                memset(recvbuf, 0, sizeof(recvbuf));
                recv(sock_fd, recvbuf, sizeof(recvbuf), 0);
                if(strcmp("abort",recvbuf))
                {
                    std::cout<<"begin work participant:"<<recvbuf<<'\n';
                    do_thing(sock_fd);
                }
                else
                {
                    std::cout<<"begin work participant:"<<recvbuf<<'\n';
                }
                
            }
            else if(strcmp("abort",recvbuf)==0)
            {
                std::cout<<"stop current transication\n";
            }
            else
            {
                std::cout<<recvbuf<<'\n';
                std::cout<<"not a vote message\n";
            }
            
        }
        memset(recvbuf, 0, sizeof(recvbuf));
        
    }
    
    return NULL;
}

void do_thing(int sock_fd)
{
    sleep(1);
    send(sock_fd, "support", strlen("support"), 0);
}