#include <iostream>
#include "Server.h"

int main() {

    Server server;

    // CREATION SERVER
    server.create_Server();

    // SET PORT OF SERVER
    server.set_address(server.getPorta());

    while(1) {
        // ACTIVE LISTEN SERVER
        server.listen_server(server.getPorta());
       // ACCEPT CLIENT CONNECTION
        server.handShake();
       // WAIT TO RECIVE DATA FROM CLIENT
        server.recv_data();
    }
    return 0;
}