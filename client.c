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
    printf("Usage: client <domain>:<port>\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
    int sockfd;
    char* port;
    char* domain;
    char* IP;
    struct sockaddr_in sa;
    struct hostent* host;

    if((argc == 1) || (strcmp(argv[1], "-h") == 0))
    {
        help();
        return EXIT_SUCCESS;
    }
    else if((argc > 2) || (!strchr(argv[1], ':')) || ((strchr(argv[1], ':'))[1] == '\0'))
    {
        printf("Bad usage.\n");
        help();
        return EXIT_SUCCESS;
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error socket creation: ");
        return EXIT_FAILURE;
    }

    // retrieve DN & port
    argv[1][strcspn(argv[1], ":")] = '\0';
    domain = argv[1];
    port = argv[1] + strcspn(argv[1], "\0") + 1;


    sa.sin_family = AF_INET;
    sa.sin_port = htons(atoi(port));
    if(!(host = gethostbyname(domain)))
    {
        fprintf(stderr, "[gethostbyname()] Error: Impossible to resolve the domain name.\n");
        return EXIT_FAILURE;
    }
    else
    {
        IP = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
        sa.sin_addr.s_addr = inet_addr(IP);
    }

    // connection
    if(connect(sockfd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
    {
        perror("[connect()] Error: ");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Connected to %s\n", IP);
    }

    // send data
    char buffer[1024];
    sprintf(buffer, "GET / HTTP/1.0\r\nHost: %s\r\n\r\n", domain);
    if(write(sockfd, buffer, strlen(buffer)) < 0)
    {
        perror("[write()] Error: ");
        return EXIT_FAILURE;
    }

    // receive data
    while(1)
    {
        if(read(sockfd, buffer, 1024) > 0)
        {
            printf("%s", buffer);
            break;
        }
    }

    // close the socket
    close(sockfd);

    return EXIT_SUCCESS;
}