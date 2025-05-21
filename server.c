#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 25015 // * You can define custom port here.

int main(){
    
    int server_fd, new_socket;
    struct sockaddr_in address; // ? Definening the socket address.
    int addrlen = sizeof(address);
    char buffer[1024] = {0}; // ? We use buffers for the data i/o's (inputs/outputs)
    char *response = "Hello, client"; 
    int opt = 1;

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
    address.sin_port = htons(PORT);

    // ? Connecting the socket to the port

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Binding error");
        exit(EXIT_FAILURE);
    }

    // ? Listening mod      
    
    if (listen(server_fd, 3) < 0)  {
        /*
        ? If there are more connections more than 3(backlog), then the other connections will get in a queue.
        ? Waiting other connections to refuse.
        ? You can play with this backlog(3) value however you want. 
        ? Specifies the maximum length of the pending connection queue.
        */
        perror("Listening error");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %d port\n", PORT);

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        /*
        ? Waiting a new connection in "server_fd" 
        ? Automatically opens a new socket to communicate with the peer(client)
        ? Sets the address to the connecting peer(client) for accepting. (It's addrlen's length bytes long) 
        */
        perror("Accept error");
        exit(EXIT_FAILURE);
    }

    // ? Reading the data from the client and sending the response.

    read(new_socket, buffer, 1024);
    // ? Reads the buffer from the "new_socket" and it returns the byte(s) (the number that it read.)
    printf("Data from client : %s\n", buffer);
    send(new_socket, response, strlen(response), 0);
    // ? Sends bytes to the "new_socket"
    printf("Response sent succesfully.\n");

    close(new_socket);
    close(server_fd);
    return 0;

}
