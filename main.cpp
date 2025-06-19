#include <iostream>
#include "Server.h"

int main() {

    Server server;
    int porta = 0;

    // CREATION SERVER
    server.create_Server();

    // SET PORT OF SERVER
    std::cout << "Inserisci la porta del tuo server: " << std::endl;
    std::cin >> porta;
    server.set_address(porta);

    while(1) {
        // ACTIVE LISTEN SERVER
        server.listen_server(porta);
       // ACCEPT CLIENT CONNECTION
        server.handShake();
       // WAIT TO RECIVE DATA FROM CLIENT
        server.recv_data();
    }


    return 0;
}