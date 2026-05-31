#include "agent.h"
#include <stdio.h>
#include <unistd.h>

int main(void) {
    int sock_fd;
    sys_metrics_t metrics;

    printf("[Miransas-Agent] Başlatılıyor...\n");

    sock_fd = init_socket();
    if (sock_fd < 0) {
        return 1;
    }

    printf("[Miransas-Agent] İzleme modu aktif. Durdurmak için: Ctrl+C\n");

    while (1) {
        collect_metrics(&metrics);
        send_metrics(sock_fd, &metrics);
        sleep(INTERVAL_SEC);
    }

    close(sock_fd);
    return 0;
}