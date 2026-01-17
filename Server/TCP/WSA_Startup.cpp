#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <atomic>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

atomic<bool> isChatOpen(true);
mutex coutMutex;

void receiveMessage(SOCKET serverSocket) {
    char buffer[512];
    while (isChatOpen) {
        int bytes = recv(serverSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            lock_guard<mutex> lock(coutMutex);
            cout << "\nClient: " << buffer << endl;
            cout << "You: " << flush;
        }
        else if (bytes == 0) {
            lock_guard<mutex> lock(coutMutex);
            cout << "\nClient disconnected\n";
            isChatOpen = false;
            break;
        }
        else {
            lock_guard<mutex> lock(coutMutex);
            cout << "\nrecv failed: " << WSAGetLastError() << endl;
            isChatOpen = false;
            break;
        }
    }
}

void sendMessage(SOCKET serverSocket) {
    char buffer[512];

    while (isChatOpen) {
        {
            lock_guard<mutex> lock(coutMutex);
            cout << "You: " << flush;
        }
        cin.getline(buffer, sizeof(buffer));
        if (strcmp(buffer, "exit") == 0) {
            isChatOpen = false;
            cout<< "Exiting...";
            shutdown(serverSocket, SD_BOTH);
            closesocket(serverSocket); 
            break;
        }

        int bytes = send(serverSocket, buffer, strlen(buffer), 0);
        if (bytes == SOCKET_ERROR) {
            lock_guard<mutex> lock(coutMutex);
            cout << "send failed: " << WSAGetLastError() << endl;
            isChatOpen = false;
            break;
        }
    }
}

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
    
    //receive data
    thread sendThread(sendMessage , acceptSOCKET);
    thread recvThread(receiveMessage , acceptSOCKET);

    sendThread.join();
    recvThread.join();
    closesocket(acceptSOCKET);
    WSACleanup();   
    return 0;
}