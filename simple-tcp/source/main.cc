#include <iostream>
#include <string>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <unistd.h>

int main(void) {
    
    int listening = socket(AF_INET, SOCK_STREAM, 0);

    if (listening == -1) {
        std::cerr << "socket failed!\n";
        return -1;
    }


    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);                   // htons: host to network short dtype.
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);  // pton: (string) pointer to number (int array).
    
    if (bind(listening, (sockaddr*) &hint, sizeof(hint)) == -1) {
        std::cerr << "cannot bind to IP/port!\n";
        return -2;
    }

    if (listen(listening, SOMAXCONN) == -1) {
        std::cerr << "cannot listen!\n";
        return -3;
    }

    sockaddr_in client;
    socklen_t client_size = sizeof(client);
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    int client_socket = accept(listening, (sockaddr*) &client, &client_size);

    if (client_socket == -1) {
        std::cerr << "client failed!\n";
        return -4;
    }

    
    close(listening);

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);


    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0);

    if (result) {
        std::cout << host << "connected on " << service << "\n";
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << "connected on " << ntohs(client.sin_port) << "\n";
    }

    char buffer[4096];
    while(true) {
        memset(buffer, 0, 4096);
        int bytes_received = recv(client_socket, buffer, 4096, 0);

        if (bytes_received == -1) {
            std::cerr << "connection error \n";
        }

        if (bytes_received == 0) {
            std::cout << " client disconnected \n";
            break;
        }

        std::cout << "received " << std::string(buffer, 0, bytes_received) << std::endl;

        send(client_socket, buffer, bytes_received + 1, 0);
    }

    close(client_socket);

    return 0;
}
