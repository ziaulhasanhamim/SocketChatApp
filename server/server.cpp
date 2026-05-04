#include <iostream>
#include <thread>
#include "ISocketDevice.hpp"
#include "AdvancedSocketServer.hpp"
using namespace std;
using namespace ChatApp::server;

int main()
{
    auto socket = CreateServerSocket("127.0.0.1", 8080);
    auto server = make_unique<AdvancedSocketServer>(socket);
    server->Start();
    return 0;
}