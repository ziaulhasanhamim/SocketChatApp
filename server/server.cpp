#include <iostream>
#include <thread>
#include "ISocketDevice.hpp"
#include <fstream>
#include "AdvancedSocketServer.hpp"
using namespace std;
using namespace ChatApp::server;

int main()
{
    std::ofstream logFile("server.log");
    try
    {
        auto socket = CreateServerSocket("127.0.0.1", 8080);
        auto server = make_unique<AdvancedSocketServer>(socket, logFile);
        server->Start();
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
    }
    logFile.close();
    return 0;
}