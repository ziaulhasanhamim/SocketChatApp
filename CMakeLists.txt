cmake_minimum_required(VERSION 3.10)
project(chatapp)
add_executable(chatserver server/server.cpp server/UnixSocket.cpp server/WindowsSocket.cpp server/SocketServer.cpp server/AdvancedSocketServer.cpp)
add_executable(chatclient client/client.cpp client/UnixSocketClient.cpp client/WindowsSocketClient.cpp)
