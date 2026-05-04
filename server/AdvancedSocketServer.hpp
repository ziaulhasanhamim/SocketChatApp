#pragma once
#include <vector>
#include <string>
#include <memory>
#include "MessageDispatcher.hpp"
#include "SocketServer.hpp"

using namespace std;

namespace ChatApp::server
{
    class AdvancedSocketServer;

    class AdvancedClientHandler : public ClientHandler
    {
        MessageDispatcher<AdvancedSocketServer> dispatcher;
        void HandleClientMessage(const string &msg, const string &sender);
        void SetupDispatcher();
    public:
        AdvancedClientHandler(const string &name, shared_ptr<ISocketDevice> s, AdvancedSocketServer *srv) : ClientHandler(s, (SocketServer *)srv)
        {
            client_name = name;
            SetupDispatcher();
        }
        void HandleClient() override;
        friend class AdvancedSocketServer;
    };

    class AdvancedSocketServer : public SocketServer
    {
    public:
        AdvancedSocketServer(shared_ptr<ISocketDevice> s) : SocketServer(s) {}
        void Start() override;
        void BroadcastMessage(const string &msg, const string &sender) override;
        void PrivateMessage(const string &msg, const string &sender, const string &recver);
        void BroadcastClientDetails();
        friend class AdvancedClientHandler;
    };
}