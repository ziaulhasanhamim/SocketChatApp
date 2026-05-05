#pragma once
#include <exception>
#include <string>
using namespace std;
namespace ChatApp::server
{
    class SocketException : public exception
    {
        string message;

    public:
        SocketException(const string &msg) : message(msg) {}
        const char *what() const noexcept override
        {
            return message.c_str();
        }
    };
}