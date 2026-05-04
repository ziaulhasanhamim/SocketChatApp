#include "SocketServer.hpp"
#include <iostream>
#include <algorithm>
#include <thread>

using namespace std;

namespace ChatApp::server
{
    ClientHandler::ClientHandler(shared_ptr<ISocketDevice> s, SocketServer *srv)
        : socket(s), server(srv) {}

    void ClientHandler::SendMessage(const string &msg)
    {
        if (socket)
        {
            (*socket) << msg;
        }
    }

    void ClientHandler::SendMessage(char* msg, size_t len)
    {
        if (socket)
        {
            socket->Send(msg,len);
        }
    }

    void ClientHandler::HandleClient()
    {
        char buffer[1024];

        int bytesReceived = socket->Receive(buffer, 1024);
        if (bytesReceived <= 0)
        {
            return;
        }
        client_name = string(buffer, bytesReceived-1);

        cout << "[Server] " << client_name << " has joined the chat." << endl;
        server->BroadcastMessage(client_name + " joined the chat", "SYSTEM");

        string content;
        while (isRunning)
        {
            int valread = socket->Receive(buffer, 1024);

            if (valread <= 0)
            {
                cout << "[Server] " << client_name << " disconnected." << endl;
                break;
            }

            for (int i = 0; i < valread; ++i)
            {
                if (buffer[i] == '\n')
                {
                    server->BroadcastMessage(content, client_name);
                    cout << "[" << client_name << "]: " << content << endl;
                    content.clear();
                }
                else
                {
                    content += buffer[i];
                }
            }
        }

        socket->Close();
    }


    SocketServer::SocketServer(shared_ptr<ISocketDevice> s)
        : serverSocket(s) {}

    void SocketServer::Start()
    {
        if (!serverSocket)
            return;

        serverSocket->Listen();
        cout << "Server started and listening for connections..." << endl;

        while (isRunning)
        {
            auto clientSock = serverSocket->Accept();

            if (clientSock)
            {
                auto handler = make_shared<ClientHandler>(clientSock, this);

                clients.push_back(handler);

                thread(&ClientHandler::HandleClient, handler).detach();
            }
        }
    }

    void SocketServer::BroadcastMessage(const string &msg, const string &sender)
    {
        string formattedMsg = sender + ": " + msg + "\n";

        for (auto& client : clients)
        {
            if (client->GetName() != sender)
            {
                client->SendMessage(formattedMsg);
            }
        }
    }
}