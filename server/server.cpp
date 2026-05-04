#include <iostream>
#include <thread>
#include "ISocketDevice.hpp"
#include "AdvancedSocketServer.hpp"
using namespace std;
using namespace ChatApp::server;

int main()
{
    auto socket = CreateServerSocket("192.168.0.192", 8080);
    auto server = make_unique<AdvancedSocketServer>(socket);
    server->Start();
    return 0;
}