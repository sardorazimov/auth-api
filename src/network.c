#include "agent.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int init_socket(void) {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("[Miransas-Network] Soket oluşturma hatası");
        return -1;
    }
    return sock_fd;
}

void send_metrics(int sock_fd, const sys_metrics_t *metrics) {
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TARGET_PORT);
    server_addr.sin_addr.s_addr = inet_addr(TARGET_IP);

    // Dış bağımlılık olmadan saf C ile JSON paketi oluşturma
    snprintf(buffer, sizeof(buffer),
             "{"
             "\"node\":\"Miransas-Node-01\","
             "\"cpu_usage_percent\":%.2f,"
             "\"total_ram_mb\":%llu,"
             "\"free_ram_mb\":%llu"
             "}",
             metrics->cpu_usage, metrics->total_ram, metrics->free_ram);

    sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("-> [UDP JSON Sent]: %s\n", buffer);
}