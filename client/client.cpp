#include "SocketClient.hpp"
#include <iostream>
#include <thread>
#ifdef _WIN32
#include "WindowsSocketClient.hpp"
#else
#include "UnixSocketClient.hpp"
#endif
using namespace std;
using namespace ChatApp::client;

int main()
{
    unique_ptr<SocketClient> client;
#ifdef _WIN32
    client = make_unique<WindowsSocketClient>("127.0.0.1", 8080);
#else
    client = make_unique<UnixSocketClient>("127.0.0.1", 8080);
#endif
    client->Connect();
    string name;
    cout << "Enter your name: ";
    
    cin >> name;
    client->SockSendMessage(name);
    thread receiveThread([&client]()
    {
        while (true)        
        {
            string message = client->ReceiveMessage();
            if (!message.empty())
            {
                cout << message << endl;
            }
        }
    });
    string message;
    while (true)
    {
        getline(cin, message);
        if (!message.empty())
        {
            client->SockSendMessage(message);
        }
    }
    return 0;
}