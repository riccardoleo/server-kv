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

KVDatabase _kvdb( "nome_db", false, KVDatabase::stChronological, 1000  );

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
    //while ((n = recv(_client_fd, tmp, sizeof(tmp), 0)) > 0) {
    //    _buf.append(tmp, n);
    //}

    const ssize_t size = recv(_client_fd, _bufChar, sizeof(_bufChar), 0);
    //std::cout << "BufferString: ";
    //std::cout << _buf << '\n';
    if (size < 0) {
        std::cout << "non ho ricevuto nullaaaa!";
    }else {
        std::cout << size << '\n';
    }

    //processMessage();

    int result = processMessageChar();

    if(result > 0) {
        perror("error message");
    }
}

void Server::processMessage() {
    std::string command;
    std::string sValue;
    std::string sKey;

    std:cout << "Start processMessage" << std::endl;
    auto start = chrono::high_resolution_clock::now();

    _kvdb.Begin();

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
            _kvdb.Set(sKey, sValue);
        } else if (command == "GET") {
            if (_kvdb.Get(sKey, sValue)) {
            } else {
            }
        } else if (command == "DEL") {
            _kvdb.Delete(sKey);
        } else {
            std::cout << "Comando selezionato non valido: " << command << '\n' << std::endl;
        }


        pos = ender + 1;
    }

    _kvdb.Commit();

    const auto end = chrono::high_resolution_clock::now();
    const chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Tempo totale: " << elapsed.count() << " ms" << std::endl;
}

int Server::processMessageChar() {

    std:cout << "Start processMessageChar" << std::endl;
    const auto start_time = chrono::high_resolution_clock::now();


    const char* start = _bufChar;
    const char* end = _bufChar + strlen(_bufChar);


    _kvdb.Begin();

    int pos = 0;
    size_t bufLen = strlen(_bufChar);
    while (pos < bufLen) {

        const char* msgStart = strchr(_bufChar + pos, '$');
        if (!msgStart) break;

        const char* msgEnd = strchr(msgStart, '\r');
        if (!msgEnd) {
            std::cout << " non trovato cr" << '\n';
            break;
        }

        size_t msgLen = msgEnd - msgStart;

        if (msgLen > MAX_BUFFER_CHAR_LENGTH) {

            std::cerr << "Messaggio troppo lungo, saltato\n";
            pos = msgEnd - _bufChar + 3;
            continue;
        }

        char message[MAX_MESSAGE_LENGTH + 1] = {0};
        if (msgLen > 1) {
            strncpy(message, msgStart + 1, msgLen - 1);
            message[msgLen - 1] = '\0';
        }

        _total_messages++;  
        int result = processSingleMessage(message);
        if(result == 0) {
            _total_messages_executed++;
        } else if(result == 1) {
            _total_messages_failed++;
        }

        pos = msgEnd - _bufChar + 1;
    }
    

    _kvdb.Commit();

    std::cout << "Total messages: " << _total_messages << std::endl;
    std::cout << "Total messages executed: " << _total_messages_executed << std::endl;
    std::cout << "Total messages failed: " << _total_messages_failed << std::endl;

    const auto end_time = chrono::high_resolution_clock::now();
    const chrono::duration<double, std::milli> elapsed = end_time - start_time;
    std::cout << "Tempo totale: " << elapsed.count() << " ms" << std::endl;

    // Azzeramento del buffer
    memset(_bufChar, 0, sizeof(_bufChar));

    return 0;
}

int Server::processSingleMessage(char singleMessage[MAX_MESSAGE_LENGTH + 1]) {
    
    size_t len = strlen(singleMessage);
    if (len > 0 && singleMessage[len - 1] == '\r') {
        singleMessage[len - 1] = '\0';
    }

    char* token = strtok(singleMessage, ":");
    if (!token) {
        return 1;
    }
    char cmd[4] = {0};
    strncpy(cmd, token, 3);

    char* key = strtok(nullptr, ":");
    if (!key) {
        return 1;
    }

    char* value = strtok(nullptr, ":");

    if (strcmp(cmd, "SET") == 0) {
        if (!value) {
            return 1;
        }
        _kvdb.Set(key, value);
    } else if (strcmp(cmd, "GET") == 0) {
        std::string value2;
        _kvdb.Get(key, value2);
    } else if (strcmp(cmd, "DEL") == 0) {
        _kvdb.Delete(key);
    } else {
        return 1;
    }

    return 0;
}
