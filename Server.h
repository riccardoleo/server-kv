//
// Created by rickleo on 6/12/25.
//

#ifndef SERVER_H
#define SERVER_H
#include <string>

#define MAX_MESSAGE_LENGTH 50
#define MAX_BUFFER_CHAR_LENGTH 50000

class Server {

public:
    void create_Server();
    void set_address(int porta);
    void listen_server(int porta) const;
    void handShake();
    void recv_data();
    int getPorta() const { return _porta; }
    int processMessageChar();
    int processSingleMessage(char[MAX_MESSAGE_LENGTH + 1]);
    int processBuffer();
    void processBuffer2();
protected:
    int _porta = 8080;
    std::string _buf;
    int _pos_buffer = 0;
    char _bufChar[MAX_BUFFER_CHAR_LENGTH];
    char message[MAX_MESSAGE_LENGTH + 1];
    int _server_fd = 0;
    int _client_fd = 0;
    int _total_messages = 0;
    int _total_messages_executed = 0;
    int _total_messages_failed = 0;
    std::string _value_str;
    int _success_msg = 0;
    int _unsuccess_msg = 0;
    int _success_buffer = 0;
    int _unsuccess_buffer = 0;
    int _bufLen = 0;

    std::string _response_batch;

    char _cmd[4] = {0};
    char _key[21] = {0};
    char _value[21] = {0};
    char* _ptr_msg = Server::message + 1; // Salta il '$'
    char* _ptr_cmd = _cmd;
    char* _ptr_key = _key;
    char* _ptr_value = _value;
    char* _ptr_buf = _bufChar;
    int _count = 0;
    int _$tart = 0;
    int _idx = 0;
private:

};

#endif //SERVER_H