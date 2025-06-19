cmake_minimum_required(VERSION 3.31)
project(V2_0_Server_sqlite3)

set(CMAKE_CXX_STANDARD 20)

add_executable(V2_0_Server_sqlite3 main.cpp
        CppSQLite3.cpp
        CppSQLite3.h
        KVDatabase.cpp
        KVDatabase.h
        Server.cpp
        Server.h
        sqlite3.c
        sqlite3.h)
