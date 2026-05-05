#pragma once
#include <string>
#include <iostream>

using namespace std;

namespace ChatApp::client
{
    class SocketClient
    {
    public:
        virtual void Connect() = 0;
        virtual void SockSendMessage(const string &message) = 0;
        virtual string ReceiveMessage() = 0;
        virtual ~SocketClient() {}
    };
}