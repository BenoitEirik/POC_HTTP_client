#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

void help()
{
    printf("Usage: server <port>\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    int sockfd;
    int a_sockfd;
    int optval = 1;
    struct sockaddr_in sa;
    socklen_t size_addr = sizeof(sa);
    char buffer[1024] = {0};

    // check arguments
    if ((argc == 1) || (strcmp(argv[1], "-h") == 0))
    {
        help();
        return EXIT_SUCCESS;
    }
    else if (argc > 2)
    {
        printf("Bad usage.\n");
        help();
        return EXIT_SUCCESS;
    }

    // socket creation
    printf("Creating server socket\n");
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error: ");
        return EXIT_FAILURE;
    }

    // set socket options
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)))
    {
        perror("Error: ");
        return EXIT_FAILURE;
    }
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(atoi(argv[1]));

    // bind socket to the port
    printf("Binding socket\n");
    if (bind(sockfd, (struct sockaddr *)&sa, size_addr) < 0)
    {
        perror("[bind()] Error: ");
        return EXIT_FAILURE;
    }

    // listening
    printf("Start listening to port %s\n", argv[1]);
    if (listen(sockfd, 4) < 0)  // WARNING last stdout line will be overwrited while listen() !
    {
        perror("[listen()] Error: ");
        return EXIT_FAILURE;
    }
    
    // accept
    if ((a_sockfd = accept(sockfd, (struct sockaddr *)&sa, &size_addr)) < 0)
    {
        perror("[accept()] accept");
        return EXIT_FAILURE;
    }
    printf("Remote addr : %s\n", inet_ntoa(sa.sin_addr));
    printf("Port: %d\n", sa.sin_port);

    // receive data
    printf("Received:\n----------------------------------------\n");
    if(read(a_sockfd, buffer, 1024) < 0)
    {
        perror("[read()] Error: ");
        return EXIT_FAILURE;
    }
    else
    {
        printf("%s\n", buffer);
    }
    printf("----------------------------------------\n");

    // send data
    if(write(a_sockfd, buffer, 1024) < 0)
    {
        perror("[write()] Error: ");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
