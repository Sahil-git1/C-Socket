#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

int main(int argc , char* argv[]){
    SOCKET clientSOCKET;
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

    clientSOCKET = INVALID_SOCKET;
    clientSOCKET = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    if(clientSOCKET == INVALID_SOCKET){
     cout<< "Error at socket() : "<< WSAGetLastError()<<endl;
     WSACleanup();
     return 0;
    }else{
       cout<< "socket is OK!"<<endl;
    } 

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    InetPton(AF_INET , "127.0.0.1" , &clientService.sin_addr.s_addr);
    clientService.sin_port = htons(port);
    if(connect(clientSOCKET , (SOCKADDR*)&clientService , sizeof(clientService)) == SOCKET_ERROR){
        cout<< "connect() failed : " << WSAGetLastError()<<endl;
        WSACleanup();
        return 0;
    }else{
        cout<< "connect() is OK!"<<endl;
    }
    system("pause");
    WSACleanup();   
    return 0;
}


