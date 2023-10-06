#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVER_IP "10.20.1.34"
#define HTTP_PORT 80
#define TCP_PORT 1337
#define UDP_PORT_1337 1337
#define UDP_PORT_9999 9999
#define BUFFER_SIZE 1024

// Error metrics
struct metrics {
    unsigned int successful_requests;
    unsigned int timeouts;
    unsigned int invalid_responses;
};

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int is_valid_http_header(char *data)
{
    return strstr(data, "HTTP/1.1") != NULL || strstr(data, "HTTP/1.0") != NULL;
}

void http_test(struct metrics *http_metrics)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    char request[] = "GET / HTTP/1.1\r\nHost: "SERVER_IP"\r\n\r\n";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(HTTP_PORT);
    inet_pton(AF_INET, SERVER_IP, &(serv_addr.sin_addr));

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    if (write(sockfd, request, strlen(request)) < 0) {
        error("ERROR writing to socket");
    }

    int n = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        error("ERROR reading from socket");
    }
    buffer[n] = '\0';

    if (is_valid_http_header(buffer)) {
        http_metrics->successful_requests++;
    } else {
        printf("Invalid HTTP Header: %s\n", buffer);
        http_metrics->invalid_responses++;
    }

    close(sockfd);
}

unsigned int calculate_checksum_tcp(const uint8_t *data, size_t length)
{
    unsigned int checksum = 0;
    for (size_t i = 0; i < length; i++) {
        checksum += (uint8_t)data[i];
    }
    return checksum;
}

void tcp_test(struct metrics *tcp_metrics)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = "Test Message";
    char receive_buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);
    inet_pton(AF_INET, SERVER_IP, &(serv_addr.sin_addr));

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    if (write(sockfd, buffer, strlen(buffer)) < 0) {
        error("ERROR writing to socket");
    }

    int n = read(sockfd, receive_buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        error("ERROR reading from socket");
    }
    receive_buffer[n] = '\0';

    unsigned int sent_checksum = calculate_checksum_tcp(buffer, strlen(buffer));
    unsigned int received_checksum = (unsigned int)strtoul(receive_buffer, NULL, 10);

    char tmp[100];
    char tmp2[100];

    sprintf(tmp, "%x", sent_checksum);
    sprintf(tmp2, "%u", received_checksum);

    if (strcmp(tmp, tmp2) == 0) {
        tcp_metrics->successful_requests++;
    } else {
        printf("Checksum mismatch. Sent: %u, Received: %u\n", sent_checksum, received_checksum);
        tcp_metrics->invalid_responses++;
    }

    close(sockfd);
}

void udp_test_1337(struct metrics *udp_1337_metrics)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = "Test Message";
    char receive_buffer[BUFFER_SIZE];
    socklen_t addrlen = sizeof(serv_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(UDP_PORT_1337);
    inet_pton(AF_INET, SERVER_IP, &(serv_addr.sin_addr));

    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR sending message");
    

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);

    struct timeval timeout;
    timeout.tv_sec = 5;  // Set your desired timeout (here: 5 seconds).
    timeout.tv_usec = 0;

    int ret = select(sockfd + 1, &fds, NULL, NULL, &timeout);
    if (ret < 0) {

        error("ERROR in select");
    } else if (ret == 0) {

        udp_1337_metrics->timeouts++;
    } else {

        // Clear the buffer
        memset(receive_buffer, 0, BUFFER_SIZE);
        ssize_t received_bytes = recvfrom(sockfd, receive_buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addrlen);
        if (received_bytes < 0) {
            error("ERROR receiving message");
        }
        
        // Verify the message by comparing it to the flipped original message.
        char flipped_buffer[BUFFER_SIZE];

        flipped_buffer[received_bytes] = '\0'; // null terminate            


        for (size_t i = 0; i < strlen(buffer); i++) {
            flipped_buffer[i] = buffer[strlen(buffer) - 1 - i];  // Bitwise NOT to flip the bits
        }
        flipped_buffer[strlen(buffer)] = '\0'; // null terminate            

        if (memcmp(receive_buffer, flipped_buffer, strlen(buffer)) == 0) {
            udp_1337_metrics->successful_requests++;
        } else {
            udp_1337_metrics->invalid_responses++;
        }
    }
    close(sockfd);
}

void udp_test_9999(struct metrics *udp_9999_metrics)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = "Test Message";
    char receive_buffer[BUFFER_SIZE];
    socklen_t addrlen = sizeof(serv_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");
    

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(UDP_PORT_9999);
    inet_pton(AF_INET, SERVER_IP, &(serv_addr.sin_addr));

    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR sending message");
    

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);

    struct timeval timeout;
    timeout.tv_sec = 2;  // Set your desired timeout (here: 5 seconds).
    timeout.tv_usec = 0;

    int ret = select(sockfd + 1, &fds, NULL, NULL, &timeout);

    if (ret < 0) {

        error("ERROR in select");
    } else if (ret == 0) {

        udp_9999_metrics->timeouts++;
    } else {

        // Reset the buffer
        memset(receive_buffer, 0, BUFFER_SIZE);

        if (recvfrom(sockfd, receive_buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addrlen) < 0) {
            error("ERROR receiving message");
        }
    
        if (strcmp(receive_buffer, "ok") != 0) {

            udp_9999_metrics->invalid_responses++;
        } else {

            udp_9999_metrics->successful_requests++;
        }
    }

    close(sockfd);
}

unsigned short calculate_checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;

    if (len == 1)
        sum += *(unsigned char*)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void icmp_test(struct metrics *icmp_metrics)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    struct icmp *icmp_header;
    socklen_t addrlen = sizeof(serv_addr);

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &(serv_addr.sin_addr));

    // Setting up ICMP header
    icmp_header = (struct icmp *)buffer;
    icmp_header->icmp_type = ICMP_ECHO;
    icmp_header->icmp_code = 0;
    icmp_header->icmp_id = getpid();  // Use PID as unique identifier
    icmp_header->icmp_seq = 1;  // Sequence number can be incremented
    icmp_header->icmp_cksum = 0;  // Reset checksum
    icmp_header->icmp_cksum = calculate_checksum(icmp_header, sizeof(struct icmp));

    if (sendto(sockfd, buffer, sizeof(struct icmp), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR sending message");
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);

    struct timeval timeout;
    timeout.tv_sec = 5;  // Set your desired timeout (here: 5 seconds).
    timeout.tv_usec = 0;

    int ret = select(sockfd + 1, &fds, NULL, NULL, &timeout);
    if (ret < 0) {
        error("ERROR in select");
    } else if (ret == 0) {
        icmp_metrics->timeouts++;
    } else {

        // Clear the buffer
        memset(buffer, 0, BUFFER_SIZE);
        if (recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addrlen) < 0) {
            error("ERROR receiving message");
        }
        struct icmp *received_icmp_header = (struct icmp *)(buffer + 20);  // Skip IP header

        if (received_icmp_header->icmp_type == ICMP_ECHOREPLY) {
            icmp_metrics->successful_requests++;
        } else {
            icmp_metrics->invalid_responses++;
        }
    }
    close(sockfd);
}

void print_summary(struct metrics *http_metrics,
                   struct metrics *tcp_metrics,
                   struct metrics *udp_1337_metrics,
                   struct metrics *udp_9999_metrics,
                   struct metrics *icmp_metrics)
{
    printf("\nMetrics Summary:\n");


    FILE *file = fopen("results.txt", "w");
    if (!file)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    unsigned int total_http = http_metrics->successful_requests + http_metrics->timeouts + http_metrics->invalid_responses;
    fprintf(file, "HTTP:     Success: %u, Timeouts: %u, Invalid responses: %u, Success rate: %.2f%%, Error rate: %.2f%%\n",
            http_metrics->successful_requests, http_metrics->timeouts, http_metrics->invalid_responses,
            total_http > 0 ? (http_metrics->successful_requests * 100.0) / total_http : 0.0,
            total_http > 0 ? (http_metrics->invalid_responses * 100.0) / total_http : 0.0);

    printf("HTTP:     Success: %u, Timeouts: %u, Invalid responses: %u, Success rate: %.2f%%, Error rate: %.2f%%\n",
        http_metrics->successful_requests, http_metrics->timeouts, http_metrics->invalid_responses,
        total_http > 0 ? (http_metrics->successful_requests * 100.0) / total_http : 0.0,
        total_http > 0 ? (http_metrics->invalid_responses * 100.0) / total_http : 0.0);

    unsigned int total_tcp = tcp_metrics->successful_requests + tcp_metrics->timeouts + tcp_metrics->invalid_responses;
    fprintf(file, "TCP:      Success: %u, Timeouts: %u, Invalid responses: %u, Success rate: %.2f%%, Error rate: %.2f%%\n",
            tcp_metrics->successful_requests, tcp_metrics->timeouts, tcp_metrics->invalid_responses,
            total_tcp > 0 ? (tcp_metrics->successful_requests * 100.0) / total_tcp : 0.0,
            total_tcp > 0 ? (tcp_metrics->invalid_responses * 100.0) / total_tcp : 0.0);

    printf("TCP:      Success: %u, Timeouts: %u, Invalid responses: %u, Success rate: %.2f%%, Error rate: %.2f%%\n",
            tcp_metrics->successful_requests, tcp_metrics->timeouts, tcp_metrics->invalid_responses,
            total_tcp > 0 ? (tcp_metrics->successful_requests * 100.0) / total_tcp : 0.0,
            total_tcp > 0 ? (tcp_metrics->invalid_responses * 100.0) / total_tcp : 0.0);
    

    unsigned int total_udp_1337 = udp_1337_metrics->successful_requests + udp_1337_metrics->timeouts + udp_1337_metrics->invalid_responses;
    fprintf(file, "UDP 1337: Success: %u, Timeouts: %u, Invalid responses: %u, Success rate: %.2f%%, Error rate: %.2f%%\n",
            udp_1337_metrics->successful_requests, udp_1337_metrics->timeouts, udp_1337_metrics->invalid_responses,
            total_udp_1337 > 0 ? (udp_1337_metrics->successful_requests * 100.0) / total_udp_1337 : 0.0,
            total_udp_1337 > 0 ? (udp_1337_metrics->invalid_responses * 100.0) / total_udp_1337 : 0.0);

    printf("UDP 1337: Success: %u, Timeouts: %u, Invalid responses: %u, Success rate: %.2f%%, Error rate: %.2f%%\n",
            udp_1337_metrics->successful_requests, udp_1337_metrics->timeouts, udp_1337_metrics->invalid_responses,
            total_udp_1337 > 0 ? (udp_1337_metrics->successful_requests * 100.0) / total_udp_1337 : 0.0,
            total_udp_1337 > 0 ? (udp_1337_metrics->invalid_responses * 100.0) / total_udp_1337 : 0.0);

    unsigned int total_udp_9999 = udp_9999_metrics->successful_requests + udp_9999_metrics->timeouts + udp_9999_metrics->invalid_responses;
    fprintf(file, "UDP 9999: Success: %u, Timeouts: %u, Invalid responses: %u, Success rate: %.2f%%, Error rate: %.2f%%\n",
            udp_9999_metrics->successful_requests, udp_9999_metrics->timeouts, udp_9999_metrics->invalid_responses,
            total_udp_9999 > 0 ? (udp_9999_metrics->successful_requests * 100.0) / total_udp_9999 : 0.0,
            total_udp_9999 > 0 ? (udp_9999_metrics->invalid_responses * 100.0) / total_udp_9999 : 0.0);
    
    printf("UDP 9999: Success: %u, Timeouts: %u, Invalid responses: %u, Success rate: %.2f%%, Error rate: %.2f%%\n",
            udp_9999_metrics->successful_requests, udp_9999_metrics->timeouts, udp_9999_metrics->invalid_responses,
            total_udp_9999 > 0 ? (udp_9999_metrics->successful_requests * 100.0) / total_udp_9999 : 0.0,
            total_udp_9999 > 0 ? (udp_9999_metrics->invalid_responses * 100.0) / total_udp_9999 : 0.0);

    unsigned int total_icmp = icmp_metrics->successful_requests + icmp_metrics->timeouts + icmp_metrics->invalid_responses;
    fprintf(file, "ICMP:     Success: %u, Timeouts: %u, Invalid responses: %u, Success rate: %.2f%%, Error rate: %.2f%%\n",
            icmp_metrics->successful_requests, icmp_metrics->timeouts, icmp_metrics->invalid_responses,
            total_icmp > 0 ? (icmp_metrics->successful_requests * 100.0) / total_icmp : 0.0,
            total_icmp > 0 ? (icmp_metrics->invalid_responses * 100.0) / total_icmp : 0.0);

    printf( "ICMP:     Success: %u, Timeouts: %u, Invalid responses: %u, Success rate: %.2f%%, Error rate: %.2f%%\n",
            icmp_metrics->successful_requests, icmp_metrics->timeouts, icmp_metrics->invalid_responses,
            total_icmp > 0 ? (icmp_metrics->successful_requests * 100.0) / total_icmp : 0.0,
            total_icmp > 0 ? (icmp_metrics->invalid_responses * 100.0) / total_icmp : 0.0);


    fclose(file);
    
}

int main()
{
    struct metrics http_metrics = {0, 0, 0};
    struct metrics tcp_metrics = {0, 0, 0};
    struct metrics udp_1337_metrics = {0, 0, 0};
    struct metrics udp_9999_metrics = {0, 0, 0};
    struct metrics icmp_metrics = {0, 0, 0};

    while (1)
    {
        http_test(&http_metrics);
        tcp_test(&tcp_metrics);
        udp_test_1337(&udp_1337_metrics);
        udp_test_9999(&udp_9999_metrics);
        icmp_test(&icmp_metrics);

        sleep(1); // Pause before the next test iteration.

        // Print summary every 60 iterations (every 60 seconds in this case).
        static int iteration_count = 0;
        if (++iteration_count == 100)
        {
            print_summary(&http_metrics, &tcp_metrics, &udp_1337_metrics, &udp_9999_metrics, &icmp_metrics);
            iteration_count = 0;
        }
    }

    return 0;
}