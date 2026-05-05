#ifndef _WIN32
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include "SocketClient.hpp"

using namespace std;

namespace ChatApp::client
{
    class UnixSocketClient : public SocketClient
    {
    private:
        int sock;
        sockaddr_in serv_addr;

    public:
        UnixSocketClient(string ip, int port);
        void Connect();
        void SockSendMessage(const string &message);
        string ReceiveMessage();
        ~UnixSocketClient();
    };
}
#endif