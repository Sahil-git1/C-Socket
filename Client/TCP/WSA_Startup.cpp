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

void receiveMessage(SOCKET clientSocket) {
    char buffer[512];
    while (isChatOpen) {
        int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            lock_guard<mutex> lock(coutMutex);
            cout << "\nServer: " << buffer << endl;
            cout << "You: " << flush;
        }
        else if (bytes == 0) {
            lock_guard<mutex> lock(coutMutex);
            cout << "\nServer disconnected\n";
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

void sendMessage(SOCKET clientSocket) {
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
            shutdown(clientSocket, SD_BOTH);
            closesocket(serverSocket); 
            break;
        }
        int bytes = send(clientSocket, buffer, strlen(buffer), 0);
        if (bytes == SOCKET_ERROR) {
            lock_guard<mutex> lock(coutMutex);
            cout << "send failed: " << WSAGetLastError() << endl;
            isChatOpen = false;
            break;
        }
    }
}

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

    // chat with server
    thread recvThread(receiveMessage , clientSOCKET);
    thread sendThread(sendMessage , clientSOCKET);

    sendThread.join();
    recvThread.join();
    closesocket(clientSOCKET);
    WSACleanup();   
    return 0;
}


