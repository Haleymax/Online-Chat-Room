#include "iostream"
#include "cstring"
#include "thread"
#include "arpa/inet.h"
#include "unistd.h"

#define BUFFSIZE 128
#define HOST_IP "127.0.0.1"
#define PORT 8082

using namespace std;

class Client
{
private:
    int sockfd;
public:
    
    Client() : sockfd(-1){}

    //连接服务器
    bool connectToServer();

    //发送消息的线程函数
    void sendMessage();

    //接收消息的线程函数
    void receiveMessage();

    //启动客户端
    void run();
};

//连接服务器
bool Client::connectToServer(){
        struct sockaddr_in serv_addr;
        bzero(&serv_addr,sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        inet_pton(AF_INET , HOST_IP , &serv_addr.sin_addr);

        sockfd = socket(AF_INET , SOCK_STREAM , 0);
        if (sockfd < 0)
        {
            perror("Fail to create socket");
            return false;
        }
        
        cout << "连接中.... " << endl;
        int ret = connect(sockfd, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr));
        if (ret < 0)
        {
            perror("Fail to connect");
            return false;
        }
        cout << "成功连接到服务器" << endl;
        return true;
    }

//发送消息的线程函数
void Client::sendMessage(){
    char name[32];
    char buf[BUFFSIZE];

    cout << "请输入你的主机名称 : ";
    cin.getline(name,32);
    write(sockfd , name , strlen(name));

    while (true)
    {   
        cout << "输入需要发送的信息：";
        cin.getline(buf,BUFFSIZE);
        write(sockfd , buf , strlen(buf));
        if (strcmp(buf,"bye") == 0)
        {
            close(sockfd);
            exit(EXIT_SUCCESS);
        }
    }
}

void Client::receiveMessage(){
    char buf[BUFFSIZE];
    while (true)
    {
        int len = read(sockfd , buf , BUFFSIZE - 1 );
        if (len > 0)
        {
            buf[len] = '\0';
            cout << "\n" << buf << endl;
            fflush(stdout);  //清空输出缓存区
        }
        
    }
    
}

//启动客户端
void Client::run(){
    if (!connectToServer())
    {
        cerr << "Failed to connect to server." << endl;
        return;
    }

    thread sendThread(&Client::sendMessage, this);
    thread receiveThread(&Client::receiveMessage, this);

    sendThread.join();
    receiveThread.join();
    
}

int main() {
    Client client;
    client.run();
    return 0;
}