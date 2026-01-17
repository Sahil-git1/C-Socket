#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

int main(int argc , char* argv[]){
    SOCKET serverSOCKET , acceptSOCKET;
    int port = 5555;
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequired = MAKEWORD(2,2);
    wsaerr = WSAStartup(wVersionRequired , &wsaData);
    if(wsaerr != 0){
     cout<<"The Winsock dll not found"<<endl;
     return 0;
    }else{
     cout<<"The Winsock dll found !"<<endl;
     cout<<"The status : " << wsaData.szSystemStatus<<endl;
    }

    serverSOCKET = INVALID_SOCKET;
    serverSOCKET = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    if(serverSOCKET == INVALID_SOCKET){
     cout<< "Error at socket() : "<< WSAGetLastError()<<endl;
     WSACleanup();
     return 0;
    }else{
       cout<< "socket is OK!"<<endl;
    } 

    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET , "127.0.0.1" , &service.sin_addr.s_addr);
    service.sin_port = htons(port);
    if(bind(serverSOCKET , (SOCKADDR*)&service , sizeof(service)) == SOCKET_ERROR){
        cout<< "bind() failed : " << WSAGetLastError()<<endl;
        closesocket(serverSOCKET);
        WSACleanup();
        return 0;
    }else{
        cout<< "bind() is OK!"<<endl;
    }

    if(listen(serverSOCKET , 1) == SOCKET_ERROR)
        cout<< "listen() error : " << WSAGetLastError()<<endl;
    else 
        cout<< "listen() is OK, waiting for connection"<<endl;    
    
    acceptSOCKET = accept(serverSOCKET , NULL , NULL);
    if(acceptSOCKET == INVALID_SOCKET){
        cout<< "accept failed: "<< WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }
    cout<< "Accepted connection"<<endl;
    system("pause");
    WSACleanup();   
    return 0;
}