
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 9000
#define BUFFER_SIZE 1024
#define NAME_SIZE 30

int send_all(int sockfd, char *data, int len) {
    int sent = 0, n;

    while (sent < len) {
        n = send(sockfd, data + sent, len - sent, 0);
        if (n <= 0)
            return -1;
        sent += n;
    }

    return 0;
}

int main() {
    int sockfd, pid, n;
    struct sockaddr_in serv_addr;
    char name[NAME_SIZE] = {0};
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        close(sockfd);
        return 1;
    }

    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    if (send_all(sockfd, name, NAME_SIZE) < 0) {
        close(sockfd);
        return 1;
    }

    printf("Connected to chat server\n");
    printf("Start chatting:\n");

    pid = fork();

    if (pid == 0) {
        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (send_all(sockfd, buffer, strlen(buffer)) < 0)
                break;
        }

        shutdown(sockfd, SHUT_WR);
        close(sockfd);
        exit(0);
    } else if (pid > 0) {
        while ((n = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
            fflush(stdout);
        }

        kill(pid, SIGTERM);
        waitpid(pid, NULL, 0);
    }

    close(sockfd);
    return 0;
}

