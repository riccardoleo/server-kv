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
    void handShake_recv_data_processBuffer();
    int getPorta() const { return _porta; }

protected:

    // LISTENER
    int _porta = 8080;
    int _server_fd = 0;
    int _client_fd = 0;

    // BUFFER
    std::string _buf;
    char _bufChar[MAX_BUFFER_CHAR_LENGTH];
    
    int _total_messages = 0;

    // MESSAGE
    char message[MAX_MESSAGE_LENGTH + 1];
    std::string _value_str;
    int _success_msg = 0;
    int _unsuccess_msg = 0;
    int _success_buffer = 0;
    int _unsuccess_buffer = 0;

    // RESPONSE
    std::string _response_batch;

    // PARSING
    char _cmd[4] = {0};
    char _key[21] = {0};
    char _value[21] = {0};

    // POINTERS
    char *_ptr_bufChar = _bufChar;
    char *_ptr_message = message;
    char *_ptr_cmd = _cmd;
    char *_ptr_key = _key;
    char *_ptr_value = _value;

    // SIZE AND COUNT
    int size = 0;
    int count = 0;

private:

};

#endif //SERVER_H