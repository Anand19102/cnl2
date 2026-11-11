
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 9000
#define BUFFER_SIZE 1024

void send_all(int sockfd, char *buffer, int size)
{
    int sent = 0;
    int n;

    while (sent < size)
    {
        n = send(sockfd, buffer + sent, size - sent, 0);

        if (n <= 0)
            return;

        sent += n;
    }
}

void handle_client(int sockfd)
{
    char filename[100];
    char buffer[BUFFER_SIZE];
    int n, used = 0;

    while ((n = recv(sockfd, filename + used,
                     sizeof(filename) - used - 1, 0)) > 0)
    {
        used += n;

        if (used >= sizeof(filename) - 1)
            break;
    }

    filename[used] = '\0';

    FILE *fp = fopen(filename, "r");

    sprintf(buffer, "Handled by Server PID : %d\n\n", getpid());
    send_all(sockfd, buffer, strlen(buffer));

    if (fp == NULL)
    {
        strcpy(buffer, "Requested File Not Found.\n");
        send_all(sockfd, buffer, strlen(buffer));
    }
    else
    {
        while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0)
        {
            send_all(sockfd, buffer, n);
        }

        fclose(fp);
    }
}

int main()
{
    int sockfd, connfd;

    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    signal(SIGCHLD, SIG_IGN);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));

    listen(sockfd, 5);

    printf("TCP Concurrent File Server Started\n");
    printf("Listening on Port %d\n", PORT);

    while (1)
    {
        connfd = accept(sockfd, (struct sockaddr *)&client, &len);

        if (connfd < 0)
            continue;

        if (fork() == 0)
        {
            close(sockfd);

            handle_client(connfd);

            close(connfd);
            exit(0);
        }

        close(connfd);
    }

    close(sockfd);

    return 0;
}

