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
    int findEmptyIndex(){
        for (size_t i = 0; i < MAXMEN; i++)
        {
            if (connfds[i] == -1)
            {
                return i;
            }
            
        }
        return -1; //没有空位置
    }
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

void Server::Start(){
    cout << "服务器启动" << endl;
    cout << "提示：输入命令“quit”以退出服务器" << endl;
    thread quitThread(&Server::quit , this);   //退出线程
    quitThread.detach();    //分离线程

    connfds.resize(MAXMEN,-1); //初始化连接数组初始值为-1

    while (true){
        struct sockaddr_in cli_addr;   //客户端信息
        socklen_t len = sizeof(cli_addr);
        int connfd = accept(listenfd , (struct sockaddr *)&cli_addr,&len);  //与客户端建立连接
        if (connfd < 0)
        {
            perror("fail to accept");
            continue;   //连接失败继续读下一次的连接
        }

        char buff[BUFFSIZE];
        time_t ticks = time(NULL);   //创建时间戳记录连接
        cout << " 连接时间 : " << ctime(&ticks) 
             << " 来自 : " << inet_ntop(AF_INET,&(cli_addr.sin_addr),buff,BUFFSIZE) 
             << " 端口 : " << ntohs(cli_addr.sin_port)
             << endl;

        int index = findEmptyIndex();
        if (index != -1)
        {
            connfds[index] = connfd;   //将套接字放入队列中
            thread clientThread(&Server::rcv_snd , this , index);  //创建线程处理客户端连接
            clientThread.detach();
        }else {
            cout << "连接已满，拒绝请求请稍后再试...." << endl;
            close(connfd);
        }
        
    }

}


