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

void Server::set_address(int porta) {

    std::cout << _server_fd << std::endl;

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(porta);

    while (bind(_server_fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0){
        porta++;
        addr.sin_port = htons(porta);
    }

    std::cout << "Hai assegnato al server la porta '" << porta << "' con successo!\n" << std::endl;

}

void Server::listen_server(const int porta) const {

    if(listen(_server_fd, SOMAXCONN) < 0){
        perror("listen");
        exit(1);
    }

    std::cout << "Server pronto su 127.0.0.1:" << _porta << std::endl;

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

    const ssize_t size = recv(_client_fd, _bufChar, sizeof(_bufChar), 0);
    if (size < 0) {
        std::cout << "non ho ricevuto nullaaaa!";
    }else {
        std::cout << size << '\n';
    }

    processBuffer2();

    std::cout << "success_msg: " << Server::_success_msg << std::endl;
    std::cout << "unsuccess_msg: " << Server::_unsuccess_msg << std::endl;
    std::cout << "success_buffer: " << Server::_success_buffer << std::endl;
    std::cout << "unsuccess_buffer: " << Server::_unsuccess_buffer << std::endl;

}

void Server::processBuffer2() {

    _response_batch.clear();
    _response_batch.reserve(250000);

    _kvdb.Begin();
    
    std::cout << "Start processBuffer2" << std::endl;
    const auto start_time = std::chrono::high_resolution_clock::now();

    while (*_ptr_buf != '\0') {

        if (_ptr_buf[_$tart] != '$') {
            _response_batch += "ERROR:";
            _response_batch += _cmd;
            _response_batch += "\n";
            Server::_unsuccess_buffer++;   
            return;
        }
        
        _ptr_msg = Server::message;
        
        while (*_ptr_buf != '\000') {

            do {

                Server::message[_idx] = *_ptr_buf;
                _idx = (_idx + 1) % MAX_MESSAGE_LENGTH;
                _ptr_buf++;
                _count++;

            } while (*_ptr_buf != '\r');
            Server::message[_idx] = '\r';

            if (Server::message[_$tart] != '$') {

                memset(Server::message, 0, _idx + 1);  

                _ptr_buf++;
                _count++;
                _idx = 0;
                continue;

            }

            _ptr_msg = Server::message;

            *_ptr_msg++;

            while (*_ptr_msg != ':' && *_ptr_msg != '\0') {
                *_ptr_cmd++ = *_ptr_msg++;
            }
            *_ptr_cmd = '\0';
            if (*_ptr_msg == ':') _ptr_msg++;
            else {
                _response_batch += "ERROR:";
                _response_batch += _cmd;
                _response_batch += "\n";
                Server::_unsuccess_msg++;
                return;
            } 

            if(strcmp(_cmd, "SET") == 0) {

                while (*_ptr_msg != ':' && *_ptr_msg != '\r' && *_ptr_msg != '\0') {
                    *_ptr_key++ = *_ptr_msg++;
                }
                *_ptr_key = '\0';

                if (*_ptr_msg == ':') _ptr_msg++;
                else {
                    _response_batch += "ERROR:";
                    _response_batch += _cmd;
                    _response_batch += "\n";
                    Server::_unsuccess_msg++;
                    return;
                } 

                while (*_ptr_msg != '\r' && *_ptr_msg != '\0') {
                    *_ptr_value++ = *_ptr_msg++;
                }
                *_ptr_value = '\0';

                _kvdb.Set(_key, _value);
                _response_batch += "OK:SET:";
                _response_batch += _key;
                _response_batch += "\n";

            }else{

                while (*_ptr_msg != '\r' && *_ptr_msg != '\0') {
                    *_ptr_key++ = *_ptr_msg++;
                }
                *_ptr_key = '\0';
                
                if (strcmp(_cmd, "GET") == 0) {
                    Server::_value_str.clear();
                    _kvdb.Get(_key, Server::_value_str);
                    _response_batch += "OK:GET:";
                    _response_batch += _key;
                    _response_batch += ":";
                    _response_batch += Server::_value_str;
                    _response_batch += "\n";
                } else {
                    _kvdb.Delete(_key);
                    _response_batch += "OK:DEL:";
                    _response_batch += _key;
                    _response_batch += "\n";
                }
            }
            

            if(*_ptr_buf == '\000') {
                break;
            }

            memset(Server::message, 0, _idx + 1);

            _ptr_buf++;
            _count++;
            _idx = 0;
            Server::_success_msg++;

            _ptr_cmd = _cmd;
            _ptr_key = _key;
            _ptr_value = _value;
            memset(_cmd, 0, sizeof(_cmd));
            memset(_key, 0, sizeof(_key));
            memset(_value, 0, sizeof(_value));
        }

        memset(Server::message, 0, _idx + 1);

        _ptr_buf++;
        _count++;
        _idx = 0;
        Server::_success_buffer++;
    }

    const auto end_time = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
    std::cout << "Tempo totale: " << elapsed.count() << " ms" << std::endl;

    _kvdb.Commit();

    memset(_bufChar, 0, sizeof(_bufChar));
    
}