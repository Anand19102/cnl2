
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <signal.h>

#define PORT 9000
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define NAME_SIZE 30

typedef struct {
    int sockfd;
    char name[NAME_SIZE];
    char buffer[BUFFER_SIZE];
    int len;
} Client;

Client clients[MAX_CLIENTS];

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

int recv_all(int sockfd, char *data, int len) {
    int received = 0, n;

    while (received < len) {
        n = recv(sockfd, data + received, len - received, 0);
        if (n <= 0)
            return -1;
        received += n;
    }

    return 0;
}

void broadcast(char *message, int sender) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd > 0 &&
            clients[i].sockfd != sender) {
            if (send_all(clients[i].sockfd, message,
                         strlen(message)) < 0) {
                close(clients[i].sockfd);
                clients[i].sockfd = 0;
            }
        }
    }
}

int main() {
    int sockfd, new_socket, max_sd, sd;
    struct sockaddr_in serv_addr;
    fd_set readfds;
    char name[NAME_SIZE];
    char temp[BUFFER_SIZE];
    char message[BUFFER_SIZE + NAME_SIZE + 5];
    int n, slot;

    signal(SIGPIPE, SIG_IGN);

    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i].sockfd = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(sockfd, 5);

    printf("Chat Server Started on Port %d\n", PORT);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        max_sd = sockfd;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = clients[i].sockfd;

            if (sd > 0) {
                FD_SET(sd, &readfds);
                if (sd > max_sd)
                    max_sd = sd;
            }
        }

        if (select(max_sd + 1, &readfds, NULL, NULL, NULL) < 0)
            continue;

        if (FD_ISSET(sockfd, &readfds)) {
            new_socket = accept(sockfd, NULL, NULL);

            if (new_socket >= 0) {
                if (recv_all(new_socket, name, NAME_SIZE) < 0) {
                    close(new_socket);
                } else {
                    name[NAME_SIZE - 1] = '\0';

                    slot = -1;

                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (clients[i].sockfd == 0) {
                            slot = i;
                            break;
                        }
                    }

                    if (slot == -1) {
                        close(new_socket);
                    } else {
                        clients[slot].sockfd = new_socket;
                        strcpy(clients[slot].name, name);
                        clients[slot].len = 0;

                        printf("%s connected\n", name);
                    }
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = clients[i].sockfd;

            if (sd > 0 && FD_ISSET(sd, &readfds)) {
                n = recv(sd, temp, sizeof(temp), 0);

                if (n <= 0) {
                    printf("%s disconnected\n", clients[i].name);
                    close(sd);
                    clients[i].sockfd = 0;
                    clients[i].len = 0;
                } else {
                    for (int j = 0; j < n; j++) {
                        if (clients[i].len < BUFFER_SIZE - 1)
                            clients[i].buffer[clients[i].len++] = temp[j];

                        if (temp[j] == '\n') {
                            clients[i].buffer[clients[i].len] = '\0';

                            snprintf(message, sizeof(message),
                                     "%s: %s",
                                     clients[i].name,
                                     clients[i].buffer);

                            printf("%s", message);
                            broadcast(message, sd);

                            clients[i].len = 0;
                        }
                    }
                }
            }
        }
    }

    close(sockfd);
    return 0;
}

