#include "client.h"

client::client(int port,string ip):server_port(port),server_ip(ip){}
client::~client(){
    close(sock);
}

void client::run(){
    //定义sockfd
    sock = socket(AF_INET,SOCK_STREAM, 0);

    //定义sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;//TCP/IP协议族
    servaddr.sin_port = htons(server_port);  //服务器端口
    servaddr.sin_addr.s_addr = inet_addr(server_ip.c_str());  //服务器ip
    //连接服务器，成功返回0，错误返回-1
    if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    cout<<"连接服务器成功！\n";
    // //创建发送线程和接收线程
    // thread send_t(SendMsg,sock),recv_t(RecvMsg,sock);
    // send_t.join();
    // cout<<"发送线程已结束"<<endl;
    // recv_t.join();
    // cout<<"接收线程已结束"<<endl;
    // return;
    
    HandleClient(sock);
    return;
}

void client::SendMsg(int conn){
    // char sendbuf[100];
    while(1){
        // memset(sendbuf,0,sizeof(sendbuf));
        // cin>>sendbuf;
        // int ret=send(conn,sendbuf,strlen(sendbuf),0);
        // if(strcmp(sendbuf,"exit")==0||ret<0){
        //     break;
        // }
        string str;
        cin>>str;
        // str="content:"+str;
        // int ret=send(conn,str.c_str(),str.length(),0);
        // if(str=="content:exit"||ret<=0){
        //     break;
        // }
        if(conn>0){
            str="content:"+str;
        }else if(conn<0){
            str="gr_message:"+str;
        }
        int ret=send(abs(conn),str.c_str(),str.length(),0);
        if(str=="content:exit"||ret<0){
            break;
        }
    }
}

void client::RecvMsg(int conn){
    char buffer[1000];
    while(1){
        memset(buffer,0,sizeof(buffer));
        int len=recv(conn,buffer,sizeof(buffer),0);
        if(len<=0){
            break;
        }
        cout<<buffer<<endl;
    }
}


void client::HandleClient(int conn){
    int choice;
    string name,pass,pass1;
    bool if_login=false;//记录是否登录成功
    string login_name;//记录成功登录的用户名

    cout<<" ------------------\n";
    cout<<"|                  |\n";
    cout<<"| 请输入你要的选项:|\n";
    cout<<"|    0:退出        |\n";
    cout<<"|    1:登录        |\n";
    cout<<"|    2:注册        |\n";
    cout<<"|                  |\n";
    cout<<" ------------------ \n\n";

    //开始处理注册、登录事件
    while(1){
        if(if_login)
           break;
        cin>>choice;
        if(choice==0)
            break;
        //注册
        else if(choice==2){
            cout<<"注册的用户名:";
            cin>>name;
            while(1){
                cout<<"密码:";
                cin>>pass;
                cout<<"确认密码:";
                cin>>pass1;
                if(pass==pass1)
                    break;
                else
                    cout<<"两次密码不一致!\n\n";
            }
            name="name:"+name;
            pass="pass:"+pass;
            string str=name+pass;
            send(conn,str.c_str(),str.length(),0);
            cout<<"注册成功！\n";
            cout<<"\n继续输入你要的选项:";
        }
        //登录
        else if(choice==1&&!if_login){
            while(1){
                cout<<"用户名:";
                cin>>name;
                cout<<"密码:";
                cin>>pass;
                string str="login"+name;
                str+="pass:";
                str+=pass;
                send(sock,str.c_str(),str.length(),0);//发送登录信息
                char buffer[1000];
                memset(buffer,0,sizeof(buffer));
                recv(sock,buffer,sizeof(buffer),0);//接收响应
                string recv_str(buffer);
                if(recv_str.substr(0,2)=="ok"){
                    if_login=true;
                    login_name=name;
                    cout<<"登录成功\n\n";
                    break;
                }
                else
                    cout<<"密码或用户名错误！\n\n";
            }
        }
    }
    //登录成功
    while(if_login&&1){
        if(if_login){
            system("clear");
            cout<<"        欢迎回来,"<<login_name<<endl;
            cout<<" -------------------------------------------\n";
            cout<<"|                                           |\n";
            cout<<"|          请选择你要的选项：               |\n";
            cout<<"|              0:退出                       |\n";
            cout<<"|              1:发起单独聊天               |\n";
            cout<<"|              2:发起群聊                   |\n";
            cout<<"|                                           |\n";
            cout<<" ------------------------------------------- \n\n";
        }
        cin>>choice;
        if(choice==0)
            break;
        //私聊
        else if(choice==1){
            cout<<"请输入对方的用户名:";
            string target_name,content;
            cin>>target_name;
            string sendstr("target:"+target_name+"from:"+login_name);//标识目标用户+源用户
            send(sock,sendstr.c_str(),sendstr.length(),0);//先向服务器发送目标用户、源用户
            cout<<"请输入你想说的话(输入exit退出)：\n";
            thread t1(client::SendMsg,conn); //创建发送线程
            thread t2(client::RecvMsg,conn);//创建接收线程
            t1.join();
            t2.join();
        }
        //群聊
        else if(choice==2){
            cout<<"请输入群号:";
            int num;
            cin>>num;
            string sendstr("group:"+to_string(num));
            send(sock,sendstr.c_str(),sendstr.length(),0);
            cout<<"请输入你想说的话(输入exit退出)：\n";
            thread t1(client::SendMsg,-conn); //创建发送线程，传入负数，和私聊区分开
            thread t2(client::RecvMsg,conn);//创建接收线程
            t1.join();
            t2.join();
        }
    }
    close(sock);
}