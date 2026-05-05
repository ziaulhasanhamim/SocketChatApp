#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include "ISocketDevice.hpp"

using namespace std;

namespace ChatApp::server
{
    class WindowsSocket : public ISocketDevice
    {
        SOCKET sock;

    public:
        WindowsSocket(SOCKET s) : sock(s) {}
        WindowsSocket(string ip, int port)
        {
            WSADATA wsa;
            WSAStartup(MAKEWORD(2, 2), &wsa);
            sock = socket(AF_INET, SOCK_STREAM, 0);
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
            auto code = ::bind(sock, (struct sockaddr *)&addr, sizeof(addr));
            if (code == SOCKET_ERROR)
            {
                throw SocketException("Failed to bind socket");
            }
        }

        void Listen() override
        {
            auto code = listen(sock, SOMAXCONN);
            if (code == SOCKET_ERROR)
            {
                throw SocketException("Failed to listen on socket");
            }
        }

        shared_ptr<ISocketDevice> Accept() override
        {
            int len = sizeof(sockaddr_in);
            sockaddr_in addr;
            SOCKET client_sock = accept(sock, (struct sockaddr *)&addr, &len);
            if (client_sock == INVALID_SOCKET)
            {
                throw SocketException("Failed to accept connection");
            }
            return make_shared<WindowsSocket>(client_sock);
        }

        bool Send(const string &data) override
        {
            return send(sock, data.c_str(), (int)data.size(), 0) > 0;
        }

        bool Send(char *data, int len) override
        {
            return send(sock, data, len, 0) > 0;
        }

        int Receive(char *buf, int len) override { return recv(sock, buf, len, 0); }
        void Close() override
        {
            closesocket(sock);
            WSACleanup();
        }
        ~WindowsSocket() { Close(); }
    };

    shared_ptr<ISocketDevice> CreateServerSocket(const string &ip, int port)
    {
        return make_shared<WindowsSocket>(ip, port);
    }
}
#endif