#include "agent.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int init_socket(void) {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("[Miransas-Network] Soket oluşturma hatası");
        return -1;
    }
    return sock_fd;
}

int send_metrics(int sock_fd, const sys_metrics_t *metrics) {
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int written;
    ssize_t sent;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TARGET_PORT);
    if (inet_pton(AF_INET, TARGET_IP, &server_addr.sin_addr) != 1) {
        fprintf(stderr, "[Miransas-Network] Gecersiz hedef IP: %s\n", TARGET_IP);
        return -1;
    }

    written = snprintf(buffer, sizeof(buffer),
                       "{"
                       "\"node\":\"Miransas-Node-01\","
                       "\"cpu_usage_percent\":%.2f,"
                       "\"total_ram_mb\":%" PRIu64 ","
                       "\"free_ram_mb\":%" PRIu64
                       "}",
                       metrics->cpu_usage, metrics->total_ram, metrics->free_ram);

    if (written < 0 || (size_t)written >= sizeof(buffer)) {
        fprintf(stderr, "[Miransas-Network] JSON buffer'a sigmadi\n");
        return -1;
    }

    sent = sendto(sock_fd, buffer, (size_t)written, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (sent < 0) {
        perror("[Miransas-Network] UDP gonderim hatasi");
        return -1;
    }

    printf("-> [UDP JSON Sent]: %s\n", buffer);
    return 0;
}
