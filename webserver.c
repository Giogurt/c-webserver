// Server side C program to demonstrate HTTP Server programming
// Libraries
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

// Constants
#define PORT 8080
#define NUM_USERS 100

// Calculates a file's size
unsigned long fsize(char* file) {

    // Opens the file and reads it
    FILE * f = fopen(file, "r");

    // Looks through all the file until the end
    fseek(f, 0, SEEK_END);

    // At the end of the file, it is saved in variable len
    unsigned long len = (unsigned long)ftell(f);
    fclose(f);

    // Returns file's size
    return len;
}

int main(int argc, char const *argv[]) {

    // Sockets we will be using
    int server_fd, new_socket;

    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // HTTP base response the client will receive
    char *str_response = "HTTP/1.1 200 OK\nContent-Type: text\n\n";
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    // Configuring the connection of the server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("0.0.0.0");
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    // Links the socket to the specified address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
        
        // Checks for error
        perror("In bind");
        exit(EXIT_FAILURE);
    }

    // Waits for the client to approach the server to make a connection,
    // also establishes the simultaneous connections the server can have
    // indicated with NUM_USERS
    if (listen(server_fd, htons(NUM_USERS)) < 0){

        // Checks for error
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    // This loop will let us keep receiving request.
    while(1) {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        
        // Here the second socket is opened to accept request made to it.
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        // We are creating a child process to manage a particular request while
        // the main program continues to accept other requests
        if(!fork()){

            // Since this is another thread we dont want to have two sockets for the server.
            close(server_fd);
            char buffer[30000] = {0};

            // The request is read
            valread = read(new_socket, buffer, 30000);
            printf("%s\n", buffer);

            // Variables to be able to read the file we are going to serve
            int html_fd, sz;

            // Save the size of the file to serve and create a buffer that will be able
            // to read that amount of data.
            long file_length = fsize("TC2025.html");
            char c[file_length];
            
            // Starts the process of opening and reading the file to serve
            html_fd = open("TC2025.html", O_RDONLY);
            if (html_fd < 0) { perror("r1"); exit(1); }
            
            sz = read(html_fd, c, file_length);
            printf("FILE WAS READ CORRECTLY - Bytes read: %d\n", sz);
            c[sz] = '\0';
            // printf("Those bytes are as follows: %s\n", c);

            // We create a new string that will contain the complete header we will be sending
            // to the client.
            char * new_str;

            // We try giving the new string a length of the base response + the 
            // length of the file
            if((new_str = malloc(strlen(str_response)+strlen(c)+1)) != NULL){
                new_str[0] = '\0';   // Ensures the memory is an empty string
                // We append the response to the new (empty) string
                strcat(new_str,str_response);
                // We append the content of the file after the response to the new string
                strcat(new_str,c);
            } else {
                perror("malloc failed!\n");
                exit(1);
            }

            // We write in the socket the complete header (the string we just created)
            // so the client can now see the file
            write(new_socket , new_str , strlen(new_str));

            printf("------------------File Served-------------------\n");
            close(new_socket);
            printf("--------------------Cerrando socket--------------------\n");
            exit(0);
        }
        close(new_socket);
    }
    return 0;
}
