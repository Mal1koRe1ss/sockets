#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 25015

int main() {
    char message [255];
    int sock = 0;
    struct sockaddr_in server_addr; // ? Defining the socket address.
    char buffer[1024] = {0}; // * We use buffers for the data i/o's (inputs/outputs)

    // ? Creating the socket

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // ? Converting the IP address to binary.

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        /*
        ? Converting the IP address from string, format that uses a ipv4 IP from the buffer.
        ? It converts the IP address to the binary network format and store results. (In struct(sin_addr))
        */
        perror("Address transfer error!");
        exit(EXIT_FAILURE);
    }

    // ? Connecting the server.

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        // ? It connects the socket to sockaddr.
        perror("Connection error.");
        exit(EXIT_FAILURE);
    }

    // ? Send messages and get the respond.
    printf("$ : ");
    scanf("%254s", message);
    send(sock, message, strlen(message), 0);
    printf("Message sended succesfully.\n");
    read(sock, buffer, 1024);
    printf("$ Response from the server : %s\n", buffer);

    close(sock);
    return 0;

}