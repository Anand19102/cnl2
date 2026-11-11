
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9000
#define BUFFER_SIZE 1024

int main()
{
    int sockfd, n;

    char filename[100];
    char buffer[BUFFER_SIZE];

    struct sockaddr_in server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&server, sizeof(server));

    printf("Enter File Name: ");
    fgets(filename, sizeof(filename), stdin);

    filename[strcspn(filename, "\n")] = '\0';

    send(sockfd, filename, strlen(filename), 0);

    shutdown(sockfd, SHUT_WR);

    printf("\nResponse Received From Server\n\n");

    while ((n = recv(sockfd, buffer, sizeof(buffer), 0)) > 0)
    {
        fwrite(buffer, 1, n, stdout);
    }

    close(sockfd);

    return 0;
}


