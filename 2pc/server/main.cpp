#include "listen_thread.cpp"
#include <pthread.h>
#include <iostream>
#include <exception>

void *func_listen(void *arg);

int main(int argc,char *argv[]){
    
    struct pthread_data pdata;
    struct params para;
    pthread_t pt;
    if(argc>1){
        int port = atoi(argv[1]);
        para.port=port;
        pthread_create(&pt, NULL, func_listen,&para);
    }
    char word[]="begin your input\n";
    char str[] = "exit";
    char vote[] = "c";
    while(1){
        char buf[100];
        
        std::cout<<"input your command\n";
        std::cin>>buf;
        if(strcmp(str,buf)==0)
        {
            break;
        }
        if(strcmp(vote,buf)==0)
        {
            std::cout<<"begin vote\n";
            para.listen.vote();
        }
    }
    return 0;
}

void *func_listen(void* arg){
    struct params * para = (struct params *)arg;
    try{
        para->listen.run(para);
    }catch(std::exception e){
        std::cout<<"监听进程终止\n";
    }
    
    
    pthread_exit(0);
}