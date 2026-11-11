
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345

void translate(char *str)
{
    struct {
        char *abbr;
        char *full;
    } dict[] = {
        {"tbh", "to be honest"},
        {"ig", "I guess"},
        {"tbf", "to be fair"},
        {"atm", "at the moment"},
        {"irl", "in real life"},
        {"lol", "laughing out loud"},
        {"asap", "as soon as possible"},
        {"omg", "oh my God"},
        {"ttyl", "talk to you later"},
        {"idk", "I don't know"},
        {"nvm", "never mind"},
        {"idc", "I don't care"}
    };

    char result[1024] = "";
    char *word;
    int i, found, used = 0;

    word = strtok(str, " \n");

    while (word != NULL) {
        found = 0;

        for (i = 0; i < 12; i++) {
            if (strcmp(word, dict[i].abbr) == 0) {
                used += snprintf(result + used, sizeof(result) - used,
                                 "%s ", dict[i].full);
                found = 1;
                break;
            }
        }

        if (!found)
            used += snprintf(result + used, sizeof(result) - used,
                             "%s ", word);

        word = strtok(NULL, " \n");
    }

    strcpy(str, result);
}

int main()
{
    int sockfd;
    char buffer[1024];
    struct sockaddr_in server_addr, client_addr;
    socklen_t len;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("UDP Server running...\n");

    len = sizeof(client_addr);

    int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                     (struct sockaddr *)&client_addr, &len);

    buffer[n] = '\0';

    printf("Received: %s\n", buffer);

    translate(buffer);

    sendto(sockfd, buffer, strlen(buffer) + 1, 0,
           (struct sockaddr *)&client_addr, len);

    close(sockfd);

    return 0;
}

