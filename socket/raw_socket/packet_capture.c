
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

#define BUFFER_SIZE 65536

void printMAC(unsigned char *mac)
{
    printf("%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2],
           mac[3], mac[4], mac[5]);
}

int main()
{
    int sockfd, data_size;
    unsigned char buffer[BUFFER_SIZE];

    struct sockaddr saddr;
    socklen_t saddr_len;

    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (sockfd < 0)
    {
        perror("Socket");
        return 1;
    }

    printf("RAW SOCKET PACKET CAPTURE\n");
    printf("Press Ctrl+C to stop...\n");

    while (1)
    {
        saddr_len = sizeof(saddr);

        data_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                             &saddr, &saddr_len);

        if (data_size < (int)sizeof(struct ethhdr))
            continue;

        struct ethhdr *eth = (struct ethhdr *)buffer;

        printf("\n-----------------------------\n");
        printf("Packet Length: %d Bytes\n", data_size);

        printf("\nEthernet Header\n");

        printf("Destination MAC: ");
        printMAC(eth->h_dest);

        printf("\nSource MAC: ");
        printMAC(eth->h_source);

        printf("\nProtocol: 0x%04X\n", ntohs(eth->h_proto));

        if (ntohs(eth->h_proto) == ETH_P_IP)
        {
            int ip_offset = sizeof(struct ethhdr);

            if (data_size < ip_offset + (int)sizeof(struct iphdr))
                continue;

            struct iphdr *iph = (struct iphdr *)(buffer + ip_offset);

            int ip_header_len = iph->ihl * 4;

            if (iph->version != 4 || ip_header_len < 20 ||
                data_size < ip_offset + ip_header_len)
                continue;

            struct in_addr src, dst;

            src.s_addr = iph->saddr;
            dst.s_addr = iph->daddr;

            printf("\nIP Header\n");
            printf("Source IP: %s\n", inet_ntoa(src));
            printf("Destination IP: %s\n", inet_ntoa(dst));
            printf("TTL: %d\n", iph->ttl);
            printf("Protocol: %d\n", iph->protocol);

            int transport_offset = ip_offset + ip_header_len;

            if (ntohs(iph->frag_off) & IP_OFFMASK)
                continue;

            if (iph->protocol == IPPROTO_TCP)
            {
                if (data_size < transport_offset + (int)sizeof(struct tcphdr))
                    continue;

                struct tcphdr *tcp =
                    (struct tcphdr *)(buffer + transport_offset);

                printf("\nTCP Header\n");
                printf("Source Port: %u\n", ntohs(tcp->source));
                printf("Destination Port: %u\n", ntohs(tcp->dest));
            }
            else if (iph->protocol == IPPROTO_UDP)
            {
                if (data_size < transport_offset + (int)sizeof(struct udphdr))
                    continue;

                struct udphdr *udp =
                    (struct udphdr *)(buffer + transport_offset);

                printf("\nUDP Header\n");
                printf("Source Port: %u\n", ntohs(udp->source));
                printf("Destination Port: %u\n", ntohs(udp->dest));
            }
            else if (iph->protocol == IPPROTO_ICMP)
            {
                if (data_size < transport_offset + (int)sizeof(struct icmphdr))
                    continue;

                struct icmphdr *icmp =
                    (struct icmphdr *)(buffer + transport_offset);

                printf("\nICMP Header\n");
                printf("Type: %d\n", icmp->type);
                printf("Code: %d\n", icmp->code);
            }
        }
    }

    close(sockfd);

    return 0;
}

