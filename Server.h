//
// Created by rickleo on 6/12/25.
//

#ifndef SERVER_H
#define SERVER_H
#include <string>

class Server {

public:
    void create_Server();
    void set_address(int porta) const;
    void listen_server(int porta) const;
    void handShake();
    void recv_data();
    void processMessage();
protected:
    std::string _buf;
    int _server_fd = 0;
    int _client_fd = 0;
private:

};

#endif //SERVER_H