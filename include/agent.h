#ifndef AGENT_H
#define AGENT_H

#include <stdint.h>

#define TARGET_IP "127.0.0.1"
#define TARGET_PORT 9999
#define INTERVAL_SEC 2
#define BUFFER_SIZE 512

typedef struct {
    uint64_t total_ram;
    uint64_t free_ram;
    double cpu_usage;
} sys_metrics_t;

int init_socket(void);
void collect_metrics(sys_metrics_t *metrics);
int send_metrics(int sock_fd, const sys_metrics_t *metrics);

#endif
