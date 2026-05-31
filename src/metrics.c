#include "agent.h"
#include <stdio.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/processor_info.h>
#include <mach/mach_host.h>

// CPU hesaplaması için bir önceki tick durumlarını tutan statik değişkenler
static unsigned long long prev_user = 0, prev_system = 0, prev_idle = 0, prev_nice = 0;

void collect_metrics(sys_metrics_t *metrics) {
    // --- 1. TOPLAM RAM HESAPLAMA ---
    int mib[2] = {CTL_HW, HW_MEMSIZE};
    int64_t physical_memory;
    size_t length = sizeof(physical_memory);
    if (sysctl(mib, 2, &physical_memory, &length, NULL, 0) == 0) {
        metrics->total_ram = physical_memory / (1024 * 1024);
    } else {
        metrics->total_ram = 0;
    }

    // --- 2. GERÇEK BOŞ RAM HESAPLAMA (macOS Sayfa Yapısı) ---
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vm_stats;
    mach_port_t host_port = mach_host_self();
    
    if (host_statistics64(host_port, HOST_VM_INFO64, (host_info64_t)&vm_stats, &count) == KERN_SUCCESS) {
        vm_size_t page_size;
        host_page_size(host_port, &page_size);
        uint64_t free_pages = vm_stats.free_count + vm_stats.speculative_count;
        metrics->free_ram = (free_pages * page_size) / (1024 * 1024);
    } else {
        metrics->free_ram = 0;
    }

    // --- 3. ANLIK CPU KULLANIMI HESAPLAMA ---
    processor_cpu_load_info_t cpu_load;
    mach_msg_type_number_t cpu_msg_count;
    unsigned int cpu_count;

    if (host_processor_info(host_port, PROCESSOR_CPU_LOAD_INFO, &cpu_count, 
                            (processor_info_array_t *)&cpu_load, &cpu_msg_count) == KERN_SUCCESS) {
        
        unsigned long long user = 0, system = 0, idle = 0, nice = 0;
        
        for (unsigned int i = 0; i < cpu_count; i++) {
            user += cpu_load[i].cpu_ticks[CPU_STATE_USER];
            system += cpu_load[i].cpu_ticks[CPU_STATE_SYSTEM];
            idle += cpu_load[i].cpu_ticks[CPU_STATE_IDLE];
            nice += cpu_load[i].cpu_ticks[CPU_STATE_NICE];
        }

        if (prev_user != 0 || prev_system != 0) {
            unsigned long long diff_user = user - prev_user;
            unsigned long long diff_system = system - prev_system;
            unsigned long long diff_idle = idle - prev_idle;
            unsigned long long diff_nice = nice - prev_nice;
            unsigned long long total_diff = diff_user + diff_system + diff_idle + diff_nice;

            if (total_diff > 0) {
                metrics->cpu_usage = ((double)(diff_user + diff_system + diff_nice) / total_diff) * 100.0;
            } else {
                metrics->cpu_usage = 0.0;
            }
        } else {
            metrics->cpu_usage = 0.0; 
        }

        prev_user = user; prev_system = system; prev_idle = idle; prev_nice = nice;
        
        vm_deallocate(mach_task_self(), (vm_address_t)cpu_load, sizeof(processor_cpu_load_info_t) * cpu_count);
    } else {
        metrics->cpu_usage = -1.0;
    }
}