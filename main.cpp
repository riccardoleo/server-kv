#include <iostream>
#include "Server.h"

int main() {

    Server server;

    // CREATION SERVER
    server.create_Server();

    // SET PORT OF SERVER
    server.set_address(server.getPorta());

    // ACTIVE LISTEN SERVER
    server.listen_server(server.getPorta());

    // HANDSHAKE, RECV DATA AND PROCESS BUFFER
    server.handShake_recv_data_processBuffer();
    
    return 0;
}