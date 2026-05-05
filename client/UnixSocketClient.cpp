#ifndef _WIN32
#include <iostream>
#include "UnixSocketClient.hpp"

using namespace std;

namespace ChatApp::client
{
    UnixSocketClient::UnixSocketClient(string ip, int port)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
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

    UnixSocketClient::~UnixSocketClient()
    {
        close(sock);
    }

    void UnixSocketClient::Connect()
    {
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            cerr << "Connection Failed" << endl;
            exit(EXIT_FAILURE);
        }
    }

    void UnixSocketClient::SockSendMessage(const string &message)
    {
        send(sock, (message + "\n").c_str(), (message + "\n").size(), 0);
    }

    string UnixSocketClient::ReceiveMessage()
    {
        char buffer[1024] = {0};
        int valread = read(sock, buffer, 1024);
        string content;
        while (valread > 0)
        {
            content += string(buffer, valread);
            if (buffer[valread - 1] == '\n')
            {
                content.pop_back(); // Remove the newline character
                break;
            }
            valread = read(sock, buffer, 1024);
        }
        
        return content;
    }
}
#endif