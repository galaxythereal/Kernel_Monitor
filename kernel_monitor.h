
#ifndef KERNEL_MONITOR_H
#define KERNEL_MONITOR_H

#include <linux/types.h>

struct system_stats {
  unsigned long cpu_usage;
  unsigned long total_ram;
  unsigned long free_ram;
  unsigned long num_processes;
  unsigned long uptime;
};

#define DEVICE_NAME "kernel_monitor"
#define CLASS_NAME "kernel_monitor_class"

#endif
