cmake_minimum_required(VERSION 3.28)

project(CLI-chat-server)

add_executable(server server.cpp IPfuncs.cpp)
add_executable(client client.cpp)