// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h> 

#define PORT 8080
#define NUM_USERS 100

unsigned long fsize(char* file)
{
    FILE * f = fopen(file, "r");
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
    fclose(f);
    return len;
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Only this line has been changed. Everything is same.
    char *str_response = "HTTP/1.1 200 OK\nContent-Type: text\n\n";
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("0.0.0.0");
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, htons( NUM_USERS )) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        if(!fork()){
            close(server_fd);
            char buffer[30000] = {0};
            valread = read(new_socket , buffer, 30000);
            printf("%s\n", buffer);

            int html_fd, sz; 

            long file_length = fsize("TC2025.html");
            char c[file_length];

            //char *c = (char *) calloc(100, sizeof(char)); 
            
            html_fd = open("TC2025.html", O_RDONLY); 
            if (html_fd < 0) { perror("r1"); exit(1); } 
            
            sz = read(html_fd, c, file_length);
            printf("FILE WAS READ CORRECTLY - Bytes read: %d\n", sz); 
            c[sz] = '\0'; 
            // printf("Those bytes are as follows: %s\n", c); 

            char * new_str ;
            if((new_str = malloc(strlen(str_response)+strlen(c)+1)) != NULL){
                new_str[0] = '\0';   // ensures the memory is an empty string
                strcat(new_str,str_response);
                strcat(new_str,c);
            } else {
                perror("malloc failed!\n");
                exit(1);
            }


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