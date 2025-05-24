#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include "include/logger.h"
#include "include/usage.h"
#include "include/utils.h"

#define DEFAULT_PORT 25015 // * You can define custom port here.
#define MAX_CLIENTS 3

int main(int argc, char *argv[]){

    char *STRINGS[] = {":close",":connections",":help",":port", ":stats"}; // ? Commands
    int server_fd, new_socket, max_sd, activity;
    struct sockaddr_in address; // ? Definening the socket address
    int client_sockets[MAX_CLIENTS] = {0};
    fd_set readfds; // ? "fd_set" for using the select & pselect.
    char buffer[1024]; // ? Using buffer for data inputs and outputs
    int port = DEFAULT_PORT;
    int opt = 1;
    int addrlen = sizeof(address);
    const char *header = "Server";
    long bytes_sent = 0; // ? Sended bytes
    int bytes_read = 0; // ? Readed bytes

    int backlog = 3; // ? Backlog value for pending connection queue.

    // ? Reading the CLI inputs at the start of the script.
    // ? Converting ASCII to Integer using "atoi()".
    if (argc > 1) port = atoi(argv[1]);

    // ? Creating the socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket didn't created!");
        exit(EXIT_FAILURE);
    }   

    // ? Enablind the SO_REUSEADDR for using the port again.
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // ? INADDR_ANY = Address to accept any incoming messages.
    address.sin_port = htons(port);
    
    // ? Connecting the socket to the port

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Binding error");
        exit(EXIT_FAILURE);
    }

    // ? Listening mod      
    
    if (listen(server_fd, backlog) < 0)  {
        /*
        ? If there are more connections more than 3(backlog), then the other connections will get in a queue.
        ? Waiting other connections to refuse.
        ? You can play with this backlog(3) value however you want. 
        ? Specifies the maximum length of the pending connection queue.
        */
        perror("Listening error");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %d port\n", port);

    while (1) {

        FD_ZERO(&readfds); // ? FD_ZERO Initializes the FD(File descriptor) set.
        FD_SET(server_fd, &readfds); // ? FD_SET represents FD(file descriptor) sets for the select function. In this example we are using it for watching the server socket.
        FD_SET(STDIN_FILENO, &readfds); // ? Watch the console inputs
        max_sd = (server_fd > STDIN_FILENO) ? server_fd : STDIN_FILENO;

        // ? Watch the client sockets
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0) FD_SET(client_sockets[i], &readfds);
            if (client_sockets[i] > max_sd) max_sd = client_sockets[i];
        }

        // ? Waiting activity with select
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("Select error");
            continue;
        }

        // ? New connection control.
        if (FD_ISSET(server_fd, &readfds)) {
            // ? Accepting the connections.
            if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
                /*
                ? Waiting a new connection in "server_fd" 
                ? Automatically opens a new socket to communicate with the peer(client)
                ? Sets the address to the connecting peer(client) for accepting. (It's addrlen's length bytes long) 
                */
                perror("Accept error");
                continue;
            }

            // ? Add the new client to the array.
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    printf("New client connected. Socket Index/FD: %d/%d\n", i,new_socket);
                    break;
                }
            }
        }   

        // ? Console input control (Sending the server message to the clients.)
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            fgets(buffer, 1024, stdin);
            buffer[strcspn(buffer, "\n")] = '\0'; // ? Delete the newline character (ENTER)
            char command[1024];
            strcpy(command, buffer);
            lowercase(command);

            //  ? Command control here
            if (strcmp(command, STRINGS[0]) == 0) { // * :close
                close(server_fd);
                exit(EXIT_SUCCESS);
            } else if (strcmp(command, STRINGS[1]) == 0) { // * :commands
                printf("Active connections:\n");
                int active_connections = 0; // ? For sending the "There is no active connections." text.

                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_sockets[i] != 0) { // ? If it's 0 there's no active connection.
                        struct sockaddr_in client_addr; // ? Socket address for client.
                        socklen_t len = sizeof(client_addr);
                        if (getpeername(client_sockets[i], (struct sockaddr*)&client_addr, &len) == 0) {
                            printf("[Index %d] FD %d | IP %s | Port: %d\n",
                                i, // ? Clients FD id
                                client_sockets[i], // ? Clients FD id
                                inet_ntoa(client_addr.sin_addr), // ? Clients ipv4 address
                                ntohs(client_addr.sin_port)); // ? Clients port
                            active_connections++;
                        }
                    }
                }   

                if (active_connections == 0) {
                    printf("There is no active connections.\n");
                }
                continue;
            } else if (strcmp(command, STRINGS[2]) == 0) { // * :help
                printf("Available commands:\n");
                printf(":close -> Closes all connection.\n");
                printf(":connections -> Shows active connections.\n");
                printf(":help -> Prints all the available commands.\n");
                printf(":port -> Shows the current port.");
                printf(":stats -> Shows data transfer statistics.\n");
                continue;
            } else if (strcmp(command, STRINGS[3]) == 0) { // * :port
                printf("Current Port : %d\n",port);
                continue;
            } else if (strcmp(command, STRINGS[4]) == 0) { // * :stats
                char stats_buffer[512];
                long ram_kb = get_memory_usage();
                double ram_mb = ram_kb / 1024.0; // * KB -> MB

                snprintf(stats_buffer, sizeof(stats_buffer),
                    "Sended Data : %.2f KB\nReceived Data : %.2f KB\nRAM Usage : %ldKB/%.2fMB\nCPU Usage : %.2f%%\n",
                    bytes_sent / 1024.0, 
                    bytes_read / 1024.0,
                    ram_kb,    // * long -> %ld
                    ram_mb,
                    get_cpu_usage()        // * float -> %.2f
                );

                printf("%s", stats_buffer); 
                continue;
            } 

            // ? Send messages to all the clients.
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] != 0) {
                    log_message(header, buffer);
                    int sent_bytes = send(client_sockets[i], buffer, strlen(buffer), 0);
                    if (sent_bytes > 0) {
                        bytes_sent += sent_bytes; // ? Adding the sended bytes to the variable.
                    }
                }
            }
        }

        // ? Check the client messages.
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] != 0 && FD_ISSET(client_sockets[i], &readfds)) {
                int read_bytes = read(client_sockets[i], buffer, 1024);
                if (read_bytes == 0) {
                    // ? Client disconnected.
                    printf("Client FD %d disconnected.\n", client_sockets[i]);
                    close(client_sockets[i]);
                    client_sockets[i] = 0;
                } else {
                    char client_header[50];
                    snprintf(client_header, sizeof(client_header), "Client FD %d", client_sockets[i]);
                    log_message(client_header,buffer);
                    buffer[read_bytes] = '\0';
                    bytes_read += read_bytes; // ? Adding the readed bytes to the variable.
                    printf("Client FD %d: %s\n", client_sockets[i], buffer);
                }
            }
        }       
    }
    
    close(server_fd);
    return 0;
}