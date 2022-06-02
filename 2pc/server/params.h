#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#ifndef params_h
#define params_h



class Coordinator{
    private:
        std::vector<int> sock_fd;


    public:
        Coordinator();
        Coordinator(std::vector<int> sock_fds);
        void add_sock(int sock_fd);
        void delete_sock(int sock_fd);
        int  size_sock_fd();
        void vote(char str[]);
        int vote_response();
        int operate_response();
        void print_sock();
        ~Coordinator();

};

class listen_thread
{
    private:
        int Port;
        std::vector<int> sock_fds;
        
    public:
        //类的构造函数
        listen_thread();
        listen_thread(int port);
        //运行线程监听
        void run();
        void run(struct params* para);
        //子线程连接
        static void *serveForClient(void *arg);
        //投票
        void vote();
        //关闭文件指针
        void Close();
        //删除套接字
        void delete_sock_fds(int sock_fd);
        //类的析构函数
        ~listen_thread();
};
struct class_params
{
    class listen_thread* pThis;
    struct sockaddr_in client_addr;
    int sock_fd;
};

struct params{
    int port;
    class Coordinator coordinator;
    class listen_thread listen;
};





 
#endif