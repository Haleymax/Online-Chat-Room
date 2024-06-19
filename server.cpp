#include <iostream>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>

#define PORT 8082   //端口
#define MAXMEN  10  //用于listen函数的最大连接数
#define BUFFSIZE 128   //缓存区大小

using namespace std;

class Server
{
private:
    int listenfd;        //监听套接字描述符
    vector<int> connfds;  //客户端套接字列表
    sockaddr_in serv_addr;   //服务器地址信息
public:

    //构造函数初始化服务器信息
    Server();
    
    //启动函数用于启动服务器
    void Start();

    //退出函数
    void quit();

    //处理客户端发送的消息的函数
    void rcv_snd(int n);

private:
    //找出连接列表中为空位置的索引
    int findEmptyIndex();
};

Server::Server(){
    listenfd = socket(AF_INET , SOCK_STREAM,0);  //创建TCP套接字
    if (listenfd < 0)
    {
        perror("fail to socket");
        exit(EXIT_FAILURE);
    }

    //初始化服务器信息
    bzero(&serv_addr , sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);   //运行所有ip访问

    int ret = bind(listenfd,(struct sockaddr *)&serv_addr , sizeof(serv_addr)); //绑定套接字
    if ( ret == -1)   
    {
        perror("fail to bind");
        exit(EXIT_FAILURE);
    }

    listen(listenfd,MAXMEN);  //将套接字设置为监听状态并设置连接队列为MAXMEN
}
