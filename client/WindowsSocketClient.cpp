#ifdef _WIN32

#include <iostream>
#include "WindowsSocketClient.hpp"

using namespace std;

namespace ChatApp::client
{
    WindowsSocketClient::WindowsSocketClient(string ip, int port)
    {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        {
            cerr << "WSAStartup failed" << endl;
            exit(EXIT_FAILURE);
        }

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET)
        {
            cerr << "Socket creation error" << endl;
            exit(EXIT_FAILURE);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
        {
            cerr << "Invalid address/ Address not supported" << endl;
            exit(EXIT_FAILURE);
        }
    }

    WindowsSocketClient::~WindowsSocketClient()
    {
        closesocket(sock);
        WSACleanup();
    }

    void WindowsSocketClient::Connect()
    {
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
        {
            cerr << "Connection Failed" << endl;
            exit(EXIT_FAILURE);
        }
    }

    void WindowsSocketClient::SockSendMessage(const string &message)
    {
        send(sock, (message + "\n").c_str(), (message + "\n").size(), 0);
    }

    string WindowsSocketClient::ReceiveMessage()
    {
        char buffer[1024] = {0};
        int valread = recv(sock, buffer, 1024, 0);
        string content;
        while (valread > 0)
        {
            content += string(buffer, valread);
            if (buffer[valread - 1] == '\n')
            {
                content.pop_back(); // Remove the newline character
                break;
            }
            valread = recv(sock, buffer, 1024, 0);
        }
        return content;
    }
}

#endif