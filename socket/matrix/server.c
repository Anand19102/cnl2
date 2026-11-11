
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX 10

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
    int server_fd, new_socket;
    struct sockaddr_in serv_addr;
    int a[MAX][MAX];
    char buffer[2048], result[100];
    int count = 0, bytes, n, i, j;
    char *p, *end;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket failed");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    listen(server_fd, 3);

    printf("Server waiting...\n");

    new_socket = accept(server_fd, NULL, NULL);

    if (new_socket < 0) {
        perror("Accept failed");
        close(server_fd);
        return 1;
    }

    while (count < sizeof(buffer) - 1 &&
           (bytes = recv(new_socket, buffer + count,
                         sizeof(buffer) - 1 - count, 0)) > 0) {
        count += bytes;
    }

    buffer[count] = '\0';

    p = buffer;
    n = strtol(p, &end, 10);

    if (p == end || n < 1 || n > MAX) {
        strcpy(result, "Invalid matrix size");
    } else {
        p = end;

        int valid = 1;

        for (i = 0; i < n && valid; i++) {
            for (j = 0; j < n; j++) {
                a[i][j] = strtol(p, &end, 10);

                if (p == end) {
                    valid = 0;
                    break;
                }

                p = end;
            }
        }

        if (!valid) {
            strcpy(result, "Invalid matrix data");
        } else {
            int upper = 1, lower = 1, diagonal = 1;

            for (i = 0; i < n; i++) {
                for (j = 0; j < n; j++) {
                    if (i > j && a[i][j] != 0)
                        upper = 0;

                    if (i < j && a[i][j] != 0)
                        lower = 0;

                    if (i != j && a[i][j] != 0)
                        diagonal = 0;
                }
            }

            if (diagonal)
                strcpy(result, "Diagonal Matrix");
            else if (upper)
                strcpy(result, "Upper Triangular Matrix");
            else if (lower)
                strcpy(result, "Lower Triangular Matrix");
            else
                strcpy(result, "Neither");
        }
    }

    send_all(new_socket, result, strlen(result) + 1);

    close(new_socket);
    close(server_fd);

    return 0;
}

