#pragma once
#include <unordered_map>
#include <functional>
#include <string>
#include "json.hpp"
using namespace std;

using json = nlohmann::json;

template <typename ServerType>
class MessageDispatcher
{
public:
    using HandlerFn =
        function<void(ServerType*, const json&, const string& sender)>;

    void Register(const string& messageType, HandlerFn handler)
    {
        handlers[messageType] = std::move(handler);
    }

    void Dispatch(ServerType* server, const json& message, const string& sender)
    {
        auto it = handlers.find(message["messageType"].get<string>());
        if (it != handlers.end())
        {
            it->second(server, message, sender);
        }
    }

private:
    unordered_map<string, HandlerFn> handlers;
};