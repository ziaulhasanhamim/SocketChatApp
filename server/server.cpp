#include <iostream>
#include <thread>
#include "ISocketDevice.hpp"
#include "AdvancedSocketServer.hpp"
using namespace std;
using namespace ChatApp::server;

int main()
{
    try
    {
        auto socket = CreateServerSocket("127.0.0.1", 8080);
        auto server = make_unique<AdvancedSocketServer>(socket);
        server->Start();
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
    }
    return 0;
}