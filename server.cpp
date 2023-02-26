#include "server.h"


server::server(int port,string ip):server_port(port),server_ip(ip){}

server::~server(){
    // for(auto conn:sock_arr){
    //     close(conn);
    // }
    // close(server_sockfd);
    for(int i=0;i<sock_arr.size();i++){
        if(sock_arr[i]){
            close(i);
        }
    }
    close(server_sockfd);
}


vector<bool> server::sock_arr(10000,false);

void server::run(){
    server_sockfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family=AF_INET;
    server_sockaddr.sin_port=htons(server_port);
    server_sockaddr.sin_addr.s_addr=inet_addr(server_ip.c_str());

    if(bind(server_sockfd,(struct sockaddr*)&server_sockaddr,sizeof(server_sockaddr))==-1){
        perror("bind");
        exit(1);
    }
    if(listen(server_sockfd,20)==-1){
        perror("listen");
        exit(1);
    }
    struct sockaddr_in client_addr;
    socklen_t length=sizeof(client_addr);
    while(1){
        int conn=accept(server_sockfd,(struct sockaddr*)&client_addr,&length);
        if(conn<0){
            perror("connect");
            exit(1);
        }
        cout<<"文件描述符为"<<conn<<"的客户端成功连接"<<endl;
        sock_arr.push_back(conn);
        thread t(server::RecvMsg,conn);
        t.detach();
    }

}

// void server::RecvMsg(int conn){
//     //接收缓冲区
//     char buffer[1000];

//     //不断接收数据
//     while(1)
//     {
//         memset(buffer,0,sizeof(buffer));
//         int len = recv(conn, buffer, sizeof(buffer),0);
//         //客户端发送exit或者异常结束时，退出
//         if(strcmp(buffer,"exit")==0 || len<=0)
//             break;
//         cout<<"收到套接字描述符为"<<conn<<"发来的消息："<<buffer<<endl;
//     }
// }

void server::RecvMsg(int conn){
    char buffer[1000];
    while(1){
        memset(buffer,0,sizeof(buffer));
        int len=recv(conn,buffer,sizeof(buffer),0);
        if(strcmp(buffer,"exit")==0||len<0){
            close(conn);
            sock_arr[conn]=false;
            break;
        }
        cout<<"收到套接字描述符为"<<conn<<"发来的消息："<<buffer<<endl;
        // string ans="收到";
        // int ret=send(conn,ans.c_str(),ans.length(),0);
        // if(ret<=0){
        //     close(conn);
        //     sock_arr[conn]=false;
        //     break;
        // }
        string str(buffer);
        HandleRequest(conn,str);
    }
}

void server::HandleRequest(int conn,string str){
    char buffer[1000];
    string name,pass;

    MYSQL * con=mysql_init(NULL);
    mysql_real_connect(con,"127.0.0.1","root","","ChatProject",0,NULL,CLIENT_MULTI_STATEMENTS);

    if(str.find("name:")!=str.npos){
        int p1=str.find("name:"),p2=str.find("pass:");
        name=str.substr(p1+5,p2-5);
        pass=str.substr(p2+5,str.length()-p2-4);
        string search="INSERT INTO USER VALUES (\"";
        search+=name;
        search+="\",\"";
        search+=pass;
        search+="\");";
        cout<<"sql语句："<<search<<endl<<endl;
        mysql_query(con,search.c_str());
    }
}