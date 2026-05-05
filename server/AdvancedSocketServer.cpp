#include <iostream>
#include "AdvancedSocketServer.hpp"
#include <thread>
#include "json.hpp"
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif // _

using json = nlohmann::json;

using namespace std;

namespace ChatApp::server
{
    void AdvancedSocketServer::Start()
    {
        serverSocket->Listen();
        logStream << "[AdvancedServer] Listening for connections..." << endl;

        while (isRunning)
        {
            auto clientSock = serverSocket->Accept();
            if (clientSock)
            {
                char size[4];
                string client_name;

                int valread = clientSock->Receive(size, sizeof(size));
                if (valread <= 0)
                {
                    clientSock->Close();
                    continue;
                }
                uint32_t msg_len = 0;
                memcpy(&msg_len, size, 4);
                msg_len = ntohl(msg_len);
                char buffer[msg_len];
                valread = clientSock->Receive(buffer, msg_len);

                if (valread <= 0)
                {
                    clientSock->Close();
                    break;
                }
                client_name = string(buffer, valread);

                logStream << "[AdvancedServer] " << client_name << " connected." << endl;
                auto isDuplicate = false;
                for (auto &client : clients)
                {
                    if (client->GetName() == client_name)
                    {
                        clientSock->Close();
                        logStream << "[AdvancedServer] duplicate connection for " << client_name << "." << endl;
                        isDuplicate = true;
                        break;
                    }
                }
                if (isDuplicate)
                    continue;
                auto handler = make_shared<AdvancedClientHandler>(client_name, clientSock, this, logStream);
                *this += handler;
                thread(&AdvancedClientHandler::HandleClient, handler).detach();
                BroadcastClientDetails();
            }
        }
    }

    void AdvancedSocketServer::BroadcastClientDetails()
    {
        vector<string> clientNames;
        for (const auto &client : clients)
        {
            clientNames.push_back(client->GetName());
        }
        json jmsg;
        jmsg["messageType"] = "client-list";
        jmsg["clients"] = clientNames;
        auto payload = jmsg.dump();
        uint32_t len = payload.size();
        uint32_t netLen = htonl(len);
        char formattedMsg[4 + payload.size()];
        memcpy(formattedMsg, &netLen, 4);
        memcpy(formattedMsg + 4, payload.data(), payload.size());
        logStream << "Broadcasting client list: " << payload << endl;

        for (auto &client : clients)
        {
            client->SockSendMessage(formattedMsg, 4 + payload.size());
        }
    }

    void AdvancedSocketServer::BroadcastMessage(const string &msg, const string &sender)
    {
        json jmsg;
        jmsg["messageType"] = "broadcast";
        jmsg["content"] = msg;
        jmsg["sender"] = sender;

        string payload = jmsg.dump();

        uint32_t len = payload.size();
        uint32_t netLen = htonl(len);
        char framedMsg[4 + payload.size()];
        memcpy(framedMsg, &netLen, 4);
        memcpy(framedMsg + 4, payload.data(), payload.size());
        logStream << "Broadcasting message from " << sender << ": " << msg << endl;
        for (auto &client : clients)
        {
            if (client->GetName() != sender)
            {
                client->SockSendMessage(framedMsg, 4 + payload.size());
            }
        }
    }

    void AdvancedSocketServer::PrivateMessage(const string &msg, const string &sender, const string &recver)
    {
        json jmsg;
        jmsg["messageType"] = "private-message";
        jmsg["content"] = msg;
        jmsg["sender"] = sender;
        auto payload = jmsg.dump();
        uint32_t len = payload.size();
        uint32_t netLen = htonl(len);
        char framedMsg[4 + payload.size()];
        memcpy(framedMsg, &netLen, 4);
        memcpy(framedMsg + 4, payload.data(), payload.size());
        logStream << "Sending private message from " << sender << " to " << recver << ": " << msg << endl;
        for (auto &client : clients)
        {
            if (client->GetName() == recver)
            {
                client->SockSendMessage(framedMsg, 4 + payload.size());
                break;
            }
        }
    }

    void AdvancedClientHandler::SetupDispatcher()
    {
        dispatcher.Register("broadcast",
                            [](AdvancedSocketServer *server, const json &jmsg, const std::string &sender)
                            {
                                server->BroadcastMessage(
                                    jmsg["content"].get<std::string>(),
                                    sender);
                            });

        dispatcher.Register("private-message",
                            [](AdvancedSocketServer *server, const json &jmsg, const std::string &sender)
                            {
                                server->PrivateMessage(
                                    jmsg["content"].get<std::string>(),
                                    sender,
                                    jmsg["receiver"].get<std::string>());
                            });
    }

    void AdvancedClientHandler::HandleClient()
    {
        string temp_buffer;
        char buffer[1024];
        while (isRunning)
        {
            int valread = socket->Receive(buffer, 1024);

            if (valread <= 0)
            {
                logStream << "[Server] " << client_name << " disconnected." << endl;
                break;
            }

            temp_buffer.append(buffer, valread);

            while (true)
            {
                if (temp_buffer.size() < 4)
                    break;

                uint32_t msg_len = 0;
                memcpy(&msg_len, temp_buffer.data(), 4);
                msg_len = ntohl(msg_len);

                if (temp_buffer.size() < 4 + msg_len)
                    break;

                string message = temp_buffer.substr(4, msg_len);

                temp_buffer.erase(0, 4 + msg_len);

                HandleClientMessage(message, client_name);
                logStream << "[" << client_name << "]: " << message << endl;
            }
        }
        isRunning = false;
        server->clients.erase(remove_if(server->clients.begin(), server->clients.end(),
                                        [this](const auto &c)
                                        { return c->GetName() == client_name; }),
                              server->clients.end());
        auto srv = dynamic_cast<AdvancedSocketServer*>(server);
        if (srv == nullptr)
        {
            logStream << "Error: Server pointer is not of type AdvancedSocketServer." << endl;
            exit(1);
        }
        srv->BroadcastClientDetails();
        logStream << "[AdvancedServer] " << client_name << " handler exiting." << endl;
    }

    void AdvancedClientHandler::HandleClientMessage(
        const std::string &msg,
        const std::string &sender)
    {
        json jmsg = json::parse(msg);
        auto srv = dynamic_cast<AdvancedSocketServer*>(server);
        if (srv == nullptr)
        {
            logStream << "Error: Server pointer is not of type AdvancedSocketServer." << endl;
            exit(1);
        }
        
        dispatcher.Dispatch(srv, jmsg, sender);
    }
}