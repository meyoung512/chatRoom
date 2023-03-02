#ifndef SERVER_H
#define SERVER_H

#include "global.h"


class server{
    private:
    int server_port;
    int server_sockfd;
    string server_ip;
    // vector<int> sock_arr;
    static vector<bool> sock_arr;
    static unordered_map<string,int> name_sock_map;
    static pthread_mutex_t name_sock_mutx;
    static unordered_map<int,set<int> > group_map;
    static pthread_mutex_t group_mutx;
    public:
    server(int port,string ip);
    ~server();
    void run();
    static void RecvMsg(int conn);
    static void HandleRequest(int conn,string str,tuple<bool,string,string,int,int> &info);
    static void setnonblocking(int conn);
};

#endif