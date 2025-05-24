#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "include/logger.h"
#include "include/usage.h"
#include "include/utils.h"

#define DEFAULT_PORT 25015

int main(int argc, char *argv[]) {

    char *STRINGS[] = {":help",":port",":disconnect",":stats"}; // ? Commands

    int sock = 0, activity;
    struct sockaddr_in server_addr; // ? Defining the socket address.
    fd_set readfds;
    char buffer[1024] = {0}; // * We use buffers for the data i/o's (inputs/outputs)
    int port = DEFAULT_PORT;
    int bytes_read = 0; // ? Readed bytes

    // ? Reading the CLI inputs at the start of the script.
    // ? Converting ASCII to Integer using "atoi()".
    if (argc > 1) port = atoi(argv[1]);

    // ? Creating the socket

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

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

    while(1) {
        FD_ZERO(&readfds); // ? FD_ZERO Initializes the FD(File descriptor) set.
        FD_SET(sock, &readfds); // ? FD_SET represents FD(file descriptor) sets for the select function. In this example we are using it for watching the socket activity.
        FD_SET(STDIN_FILENO, &readfds); // ? Watching the console inputs .
        int max_sd = (sock > STDIN_FILENO) ? sock : STDIN_FILENO;

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("Select error");
            continue;
        }

        // ? Get the data from the server
        if (FD_ISSET(sock, &readfds)) {
            // ? You can think the "FD_ISSET" as FD_SET is set.
            int read_bytes = read(sock, buffer, 1024);
            if (read_bytes <= 0) {
                printf("Server closed the connection.\n");
                break;
            }
            buffer[read_bytes] = '\0';
            printf("Server: %s\n", buffer);

            if (bytes_read > 0) {
                bytes_read += read_bytes; // ? Adding the sended bytes to the variable.
            }    
        }

        // ? Send data from the console.
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            // ? STDIN_FILENO species the FD(file descriptor)s for standard input and outputs.
            fgets(buffer, 1024, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            char command[1024];
            strcpy(command, buffer);
            lowercase(command);

            // ? Command control here

            if (strcmp(command, STRINGS[0]) == 0) {
                printf("Available commands:\n");
                printf(":disconnect -> Disconnects from the server.\n");
                printf(":port -> Shows the used port.\n");
                printf(":help -> Prints all the available commands.\n");
                continue;
            } else if (strcmp(command, STRINGS[1]) == 0) {
                printf("Current Port : %d\n",port);
                continue;
            } else if (strcmp(command, STRINGS[2]) == 0) {
                close(sock);
                exit(EXIT_SUCCESS);
            } else if (strcmp(command, STRINGS[3]) == 0) {
                char stats_buffer[512];
                long ram_kb = get_memory_usage();
                double ram_mb = ram_kb / 1024.0; // KB -> MB

                snprintf(stats_buffer, sizeof(stats_buffer),
                    "Received Data : %.2f KB\nRAM Usage : %ldKB/%.2fMB\nCPU Usage : %.2f%%\n",
                    bytes_read / 1024.0,
                    ram_kb,    // long -> %ld
                    ram_mb,
                    get_cpu_usage()        // float -> %.2f
                );

                printf("%s", stats_buffer); 
                continue;
            } else {
                send(sock, buffer, strlen(buffer), 0); // ? If there's no command used we just send the message.
                continue;
            }
        }
    }

    return 0;
}