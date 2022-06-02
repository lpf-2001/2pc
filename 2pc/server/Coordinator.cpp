#include <vector>
#include "params.h"
#include <iostream>





Coordinator::Coordinator()
{

}

Coordinator::Coordinator(std::vector<int>sock_fds)
{
    this->sock_fd = sock_fds;
}


void Coordinator::add_sock(int sock_fd)
{
    this->sock_fd.push_back(sock_fd);
}

void Coordinator::delete_sock(int sock_fd)
{
    std::cout<<"Coordinator I prepare delete this socket:"<<sock_fd<<'\n';
    for(std::vector<int>::iterator iter=this->sock_fd.begin();iter!=this->sock_fd.end();iter++)
    {        //从vector中删除指定的某一个元素 
        // std::cout<<"vector sockfd address"<<&this->sock_fd[0]<<'\n';     //打印this的vector首地址
        if(*iter==sock_fd){
            std::cout<<"find it\n";
            try{
                this->sock_fd.erase(iter);
            }catch(std::exception e){
                std::cout<<"erase fault\n";
            }
            
        break;
        }
    }
    
}

int Coordinator::size_sock_fd()
{
    return this->sock_fd.size();
}

void Coordinator::print_sock()
{
    for(int i=0;i<this->sock_fd.size();i++)
    {
        std::cout<<this->sock_fd[i]<<'\n';
    }
}

void Coordinator::vote(char str[])
{
    for(int i=0 ;i < this->sock_fd.size();i++)
    {
        std::cout<<"Coordinator sending message:"<<str<<" to socket:"<<this->sock_fd[i]<<'\n';       //打印所有的socket
        send(this->sock_fd[i], str, strlen(str), 0);
    }
}

int Coordinator::vote_response()
{
    char recvbuf[1024];
    int index=0;
    for(std::vector<int>::iterator iter=this->sock_fd.begin();iter!=this->sock_fd.end();iter++)
    {   //接收来自所有participant的ack信息
        memset(recvbuf,0,sizeof(recvbuf));
        index++;
        if(recv(*iter, recvbuf, sizeof(recvbuf), 0)==0)    //连接如果断开就abort
        {
            std::cout<<"in prepare phase this socket:"<<*iter<<" connection close\n";
            int temp_sock=*iter;
            delete_sock(*iter);
            char temp[]="abort";
            vote(temp);       //发送abort信息
            return temp_sock;
        }
        
        std::cout<<"participant:"<<index<<" sending message:"<<recvbuf<<'\n';
        if(strcmp("get",recvbuf))  //如果有任意一个participant未发送确认消息，就abort
        {
            std::cout<<"abort commit\n";
            char temp[]="abort";
            vote(temp);       //发送abort信息
            return 0;
        }
    }
    memset(recvbuf,0,sizeof(recvbuf));
    return 1;
}

int Coordinator::operate_response()
{
    int index=0;
    char recvbuf[1024];
    for(std::vector<int>::iterator iter=this->sock_fd.begin();iter!=this->sock_fd.end();iter++)
    {   //接收来自所有participant的操作情况信息
        
        index++;
        memset(recvbuf,0,sizeof(recvbuf));
        if(recv(*iter, recvbuf, sizeof(recvbuf), 0)==0)   //连接断开就abort
        {
            std::cout<<"Coordinator in commit phase this socket:"<<*iter<<" connection close\n";
            int temp_sock=*iter;
            delete_sock(*iter);
            char temp[]="abort";
            vote(temp);       //发送abort信息
            return temp_sock;
        }
        
        std::cout<<"participant:"<<index<<" sending message:"<<recvbuf<<'\n';
        if(strcmp("support",recvbuf))  //如果有任意一个participant未发送support消息，就abort
        {
            
            std::cout<<"a participant abort this transiciation\n";
            char temp[]="abort";
            vote(temp);       //发送abort信息
            return 0;
        }
    }
    memset(recvbuf,0,sizeof(recvbuf));
    return 1;
}

Coordinator::~Coordinator()
{

}