//
// Created by rickleo on 6/12/25.
//

#include "Server.h"

#include <chrono>

#include "KVDatabase.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <ratio>

KVDatabase key_value_db( "nome_db", false, KVDatabase::stChronological, 1000  );

void Server::create_Server(){
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << _server_fd << std::endl;
    if(_server_fd < 0){
        perror("socket");
        exit(1);
    }

    std::cout << "Server socket creato con successo!" << std::endl;
}

void Server::set_address(const int porta) const {

    std::cout << _server_fd << std::endl;

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(porta);

    if(bind(_server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0){
        perror("bind");
        exit(1);
    }

    std::cout << "Hai assegnato al server la porta '" << porta << "' con successo!\n" << std::endl;

}

void Server::listen_server(const int porta) const {

    if(listen(_server_fd, SOMAXCONN) < 0){
        perror("listen");
        exit(1);
    }

    std::cout << "Server pronto su 127.0.0.1:" << porta << std::endl;

}

void Server::handShake() {

    _client_fd = accept(_server_fd, nullptr, nullptr);
    if(_client_fd < 0){
        perror("accept");
        exit(1);
    }

    std::cout << "Cliente connesso\n" << std::endl;

}

void Server::recv_data() {

    char tmp[512];
    ssize_t n;
    while ((n = recv(_client_fd, tmp, sizeof(tmp), 0)) > 0) {
        _buf.append(tmp, n);
    }

    processMessage();

}

void Server::processMessage() {
    std::string command;
    std::string sValue;
    std::string sKey;

    std:cout << "Start processMessage" << std::endl;
    auto start = chrono::high_resolution_clock::now();

    key_value_db.Begin();

    size_t starter, ender;

    size_t pos = 0;
    while ((starter = _buf.find('$', pos)) != std::string::npos && (ender = _buf.find('\r', starter + 1)) != std::string::npos) {

        std::string frame = _buf.substr(starter + 1, ender - starter - 1);
        _buf.erase(0, ender + 1);
        if (const size_t sep = frame.find(':'); sep != std::string::npos) {
            command = frame.substr(0, sep);
            if (const size_t sep2 = frame.find(':', sep + 1); sep2 != std::string::npos) {
                sKey = frame.substr(sep + 1, sep2 - sep - 1);
                sValue = frame.substr(sep2 + 1);
            } else {
                sKey = frame.substr(sep + 1);
                sValue = "";
            }
        } else {
            continue;
        }
        if (command == "SET") {
            key_value_db.Set(sKey, sValue);
        } else if (command == "GET") {
            if (key_value_db.Get(sKey, sValue)) {
            } else {
            }
        } else if (command == "DELETE") {
            key_value_db.Delete(sKey);
        } else {
            std::cout << "Comando selezionato non valido: " << command << '\n' << std::endl;
        }


        pos = ender + 1;
    }

    key_value_db.Commit();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Tempo totale: " << elapsed.count() << " ms" << std::endl;
}