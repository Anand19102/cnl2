
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
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
    int sockfd, n, i, j;
    struct sockaddr_in serv_addr;
    int a[MAX][MAX];
    char buffer[2048], result[100];
    int pos = 0, bytes, total = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("Socket failed");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }

    printf("Enter order of matrix (N): ");
    scanf("%d", &n);

    if (n < 1 || n > MAX) {
        printf("Invalid matrix size\n");
        close(sockfd);
        return 1;
    }

    srand(time(NULL));

    pos += sprintf(buffer + pos, "%d ", n);

    printf("\nGenerated Matrix:\n");

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            a[i][j] = rand() % 51;

            printf("%3d ", a[i][j]);

            pos += sprintf(buffer + pos, "%d ", a[i][j]);
        }
        printf("\n");
    }

    if (send_all(sockfd, buffer, pos) < 0) {
        printf("Sending failed\n");
        close(sockfd);
        return 1;
    }

    shutdown(sockfd, SHUT_WR);

    while ((bytes = recv(sockfd, result + total,
                         sizeof(result) - 1 - total, 0)) > 0) {
        total += bytes;
        if (total == sizeof(result) - 1)
            break;
    }

    result[total] = '\0';

    printf("\nMatrix Type: %s\n", result);

    close(sockfd);

    return 0;
}

