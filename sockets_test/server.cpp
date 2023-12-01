#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define BACKLOG 5
#define BUFFER_SIZE 1024

void send_file(int client_socket, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(client_socket, buffer, bytesRead, 0) == -1) {
            perror("Error sending file");
            break;
        }
    }

    fclose(file);
}

int main() {
    int server_socket, client_socket1, client_socket2;
    struct sockaddr_in server_address, client_address1, client_address2;
    socklen_t client_addr_len = sizeof(client_address1);

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure the server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Bind the socket to a specific address and port
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, BACKLOG) == -1) {
        perror("Error listening for connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept two incoming connections
    client_socket1 = accept(server_socket, (struct sockaddr*)&client_address1, &client_addr_len);
    client_socket2 = accept(server_socket, (struct sockaddr*)&client_address2, &client_addr_len);

    if (client_socket1 == -1 || client_socket2 == -1) {
        perror("Error accepting connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from client 1: %s:%d\n", inet_ntoa(client_address1.sin_addr), ntohs(client_address1.sin_port));

    // Send file1.txt to client 1
    send_file(client_socket1, "data/test.01.csv");

    printf("Connection accepted from client 2: %s:%d\n", inet_ntoa(client_address2.sin_addr), ntohs(client_address2.sin_port));

    // Send file2.txt to client 2
    send_file(client_socket2, "data/test.00.csv");

    // Close the sockets
    close(client_socket1);
    close(client_socket2);
    close(server_socket);

    return 0;
}
