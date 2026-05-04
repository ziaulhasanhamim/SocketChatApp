#pragma once
#include <vector>
#include <string>

using namespace std;

namespace ChatApp::server
{
    class ISocketDevice
    {
    public:
        virtual ~ISocketDevice() {}
        virtual bool Send(const std::string &data) = 0;
        virtual bool Send(char* data, int len) = 0;
        virtual int Receive(char *buffer, int len) = 0;
        virtual void Close() = 0;
        virtual void Listen() = 0;
        virtual shared_ptr<ISocketDevice> Accept() = 0;
        void operator<<(const string &data)
        {
            Send(data);
        }
    };
    
    shared_ptr<ISocketDevice> CreateServerSocket(const std::string& ip, int port);
}