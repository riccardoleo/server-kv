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
    void set_address(int porta) const;
    void listen_server(int porta) const;
    void handShake();
    void recv_data();
    void processMessage();
    int processMessageChar();
    int processSingleMessage(char[MAX_MESSAGE_LENGTH + 1]);
protected:
    std::string _buf;
    char _bufChar[MAX_BUFFER_CHAR_LENGTH];
    int _server_fd = 0;
    int _client_fd = 0;
    int _total_messages = 0;
    int _total_messages_executed = 0;
    int _total_messages_failed = 0;
private:

};

#endif //SERVER_H