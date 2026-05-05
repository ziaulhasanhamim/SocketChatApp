#pragma once

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "SocketClient.hpp"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

namespace ChatApp::client
{
    class WindowsSocketClient : public SocketClient
    {
    private:
        SOCKET sock;
        sockaddr_in serv_addr;

    public:
        WindowsSocketClient(string ip, int port);
        void Connect();
        void SockSendMessage(const string &message);
        string ReceiveMessage();
        ~WindowsSocketClient();
    };
}

#endif