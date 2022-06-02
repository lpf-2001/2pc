#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <exception>
#include "params.h"
#include "Coordinator.cpp"
struct pthread_data{
            struct sockaddr_in client_addr;
            int sock_fd;

};
pthread_mutex_t g_mutext;

//文件操作类
listen_thread::listen_thread()
{
    this->Port=8080;
}

listen_thread::listen_thread(int port)
{
    this->Port=port;
}
/*void listen_thread::run(){
    int sock_fd;
    struct sockaddr_in mysock;
    struct class_params pdata;
    pthread_t pt;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr;
    int new_fd;
    //初始化socket
    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    //编辑地址信息
    memset(&mysock, 0, sizeof(mysock));
    mysock.sin_family = AF_INET;
    mysock.sin_port = htons(this->Port);
    mysock.sin_addr.s_addr = INADDR_ANY;
    
    //绑定地址，然后监听
    bind(sock_fd,(struct sockaddr *)&mysock,sizeof(struct sockaddr));
    if(listen(sock_fd,10) < -1){
        printf("listen error.\n");
    }
    
    printf("listening...\n");
    while(1){
        //accpet
        new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
        pdata.client_addr = client_addr;
        pdata.sock_fd = new_fd;
        pthread_create(&pt, NULL, serveForClient, (void *)&pdata);
    }
    close(new_fd);
    close(sock_fd);
}
*/


void listen_thread::run(struct params* para){
    this->Port=para->port;
    int sock_fd;
    struct sockaddr_in mysock;
    struct class_params pdata;
    pthread_t pt;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr;
    int new_fd;
    //初始化socket
    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    //编辑地址信息
    memset(&mysock, 0, sizeof(mysock));
    mysock.sin_family = AF_INET;
    mysock.sin_port = htons(this->Port);
    mysock.sin_addr.s_addr = INADDR_ANY;
    
    //绑定地址，然后监听
    bind(sock_fd,(struct sockaddr *)&mysock,sizeof(struct sockaddr));
    if(listen(sock_fd,10) < -1){
        printf("listen error.\n");
    }
    pdata.pThis=this;
    
    // std::cout<<"this address:"<<this<<'\n';
    while(1){
        //accpet
        printf("listening...\n");
        new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);

        this->sock_fds.push_back(new_fd);
        int i=0;
        for(std::vector<int>::iterator iter=this->sock_fds.begin();iter!=this->sock_fds.end();iter++){        //打印this的vector里面的所有iter
                
                std::cout<<"this->vector iter:"<<i<<" address："<<&*iter<<'\n';
                i++;
        }


        // std::cout<<"this->vector address:"<<&this->sock_fds[0]<<'\n';
        // std::cout<<"this->sock_fds[0]:"<<this->sock_fds[0]<<'\n';
        //para->coordinator.add_sock(new_fd); //作用待定
        pdata.client_addr = client_addr;
        pdata.sock_fd = new_fd;
        //pthread_create(&pt, NULL, serveForClient, (void *)&pdata);
    }
    close(new_fd);
    close(sock_fd);
}

void listen_thread::delete_sock_fds(int sock_fd)
{
    std::cout<<"I prepare delete this socket:"<<sock_fd<<'\n';
    for(std::vector<int>::iterator iter=this->sock_fds.begin();iter!=this->sock_fds.end();iter++)
    {        //从vector中删除指定的某一个元素 
        // std::cout<<"vector sockfd address"<<&this->sock_fd[0]<<'\n';     //打印this的vector首地址
        if(*iter==sock_fd){
            std::cout<<"find it\n";
            try{
                this->sock_fds.erase(iter);
            }catch(std::exception e){
                std::cout<<"erase fault\n";
            }
            
        break;
        }
    }
}

void *listen_thread::serveForClient(void *arg){
    //printf  ("listen_thread_server tid is %lld \n " ,   pthread_self ());   //打印线程Id

    struct class_params *pdata = (struct class_params*)arg;
    int new_fd = pdata->sock_fd;
    char recvbuf[1024];
    std::cout<<"pthread pThis address"<<pdata->pThis<<'\n';
    while(1){
        recv(new_fd, recvbuf, sizeof(recvbuf), 0);
        fputs(recvbuf,stdout);
        if(strcmp(recvbuf,"exit\n") == 0){
            
        }
        memset(recvbuf,0,sizeof(recvbuf));
    }
    pthread_exit(0);
}

void listen_thread::vote()
{
    char recvbuf[1024];
    Coordinator coordinator(this->sock_fds);
    std::cout<<"current participant numbers:"<<coordinator.size_sock_fd()<<'\n';   //打印参与投票的所有参与者的个数
    char vote[]="vote";
    coordinator.vote(vote);   //由协调者发布投票
    int index=0;
    //当所有participant返回ack消息后，开始接收所有participant的操作信息
    int response_result=coordinator.vote_response();
    if(response_result==1)
    {
        std::cout<<"sending response message to all participants:"<<std::endl;
        char all[]="1111";
        coordinator.vote(all);           //服务器发送ack确认收到客户端节点的get
        int operate_response_result=coordinator.operate_response();
        if(operate_response_result==1)    //客户端节点处理事务
        {
            //这是一次成功操作，可以commit
            std::cout<<"commit transaction\n";
        }
        else if(operate_response_result==0)
        {
            std::cout<<"abort transaction\n";
        }
        else
        {
            std::cout<<"abort transaction\n";
            delete_sock_fds(operate_response_result);            //删除
            std::cout<<"Listen_thread: in commit phase delete this closed sock_fd:"<<response_result<<'\n';
        }
    }
    else if(response_result==0)
    {
        std::cout<<"abort transaction\n";
    }
    else
    {
        std::cout<<"abort transaction\n";
        delete_sock_fds(response_result);            //删除
        std::cout<<"Listen_thread: in prepare phase delete this closed sock_fd:"<<response_result<<'\n';
    }

    

}



listen_thread::~listen_thread()
{

}