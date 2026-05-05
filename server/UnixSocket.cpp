#pragma once
#ifndef _WIN32
#include "ISocketDevice.hpp"
#include <sys/socket.h>
#include "SocketException.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

namespace ChatApp::server
{
    class UnixSocket : public ISocketDevice
    {
        int fd;

    public:
        UnixSocket(int f) : fd(f) {}
        UnixSocket(string ip, int port)
        {
            fd = socket(AF_INET, SOCK_STREAM, 0);
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
            auto code = ::bind(fd, (struct sockaddr *)&addr, sizeof(addr));
            if (code < 0)
            {
                throw SocketException("Failed to bind socket");
            }
        }

        void Listen() override
        {
            auto code = listen(fd, SOMAXCONN);
            if (code < 0)
            {
                throw SocketException("Failed to listen on socket");
            }
        }

        shared_ptr<ISocketDevice> Accept() override
        {
            sockaddr_in addr;
            socklen_t len = sizeof(addr);
            int client_fd = accept(fd, (struct sockaddr *)&addr, &len);
            if (client_fd < 0)
            {
                throw SocketException("Failed to accept connection");
            }
            return make_shared<UnixSocket>(client_fd);
        }

        bool Send(const string &data) override
        {
            return write(fd, data.c_str(), data.size()) > 0;
        }

        bool Send(char* data, int len) override
        {
            return write(fd, data, len) > 0;
        }
        
        int Receive(char *buf, int len) override
        {
            return read(fd, buf, len);
        }
        void Close() override
        {
            close(fd);
        }
        ~UnixSocket() { Close(); }
    };
    
    shared_ptr<ISocketDevice> CreateServerSocket(const string &ip, int port)
    {
        return make_shared<UnixSocket>(ip, port);
    }
}
#endif