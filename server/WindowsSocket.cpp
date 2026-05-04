#ifdef _WIN32
#include "SocketFactory.h"
#include <winsock2.h>

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
            ::bind(sock, (struct sockaddr *)&addr, sizeof(addr));
        }

        void Listen() override { listen(sock, SOMAXCONN); }

        shared_ptr<ISocketDevice> Accept() override
        {
            int len = sizeof(sockaddr_in);
            sockaddr_in addr;
            SOCKET client_sock = accept(sock, (struct sockaddr *)&addr, &len);
            return (client_sock == INVALID_SOCKET) ? nullptr : make_shared<WindowsSocket>(client_sock);
        }

        bool Send(const string &data) override
        {
            return send(sock, data.c_str(), (int)data.size(), 0) > 0;
        }

        bool Send(char* data, int len) override
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

    shared_ptr<ISocketDevice> SocketFactory::CreateServerSocket(const string &ip, int port)
    {
        return make_shared<WindowsSocket>(ip, port);
    }
}
#endif