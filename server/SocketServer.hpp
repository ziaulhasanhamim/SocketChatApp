#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ISocketDevice.hpp"

using namespace std;

namespace ChatApp::server
{
    class SocketServer;

    class ClientHandler
    {
    protected:
        string client_name;
        shared_ptr<ISocketDevice> socket;
        SocketServer *server;
        bool isRunning = true;

    public:
        ClientHandler(shared_ptr<ISocketDevice> s, SocketServer *srv);
        virtual void HandleClient();
        virtual void SendMessage(const string &msg);
        void SendMessage(char* msg, size_t len);
        virtual const string &GetName() const { return client_name; }
        ~ClientHandler() { isRunning = false; }
        friend class SocketServer;
    };

    class SocketServer
    {
    protected:
        vector<shared_ptr<ClientHandler>> clients;
        shared_ptr<ISocketDevice> serverSocket;
        bool isRunning = true;

    public:
        SocketServer(shared_ptr<ISocketDevice> s);
        virtual void Start();
        virtual void BroadcastMessage(const string &msg, const string &sender);
        ~SocketServer() { isRunning = false; }
        void operator+=(shared_ptr<ClientHandler> client)
        {
            clients.push_back(client);
        }
        void operator<<(const string &msg)
        {
            BroadcastMessage(msg, "Server");
        }
        friend class ClientHandler;
        friend class AdvancedClientHandler;
    };
}