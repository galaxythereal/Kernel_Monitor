/**
 * @file kernel_monitor.c
 * @brief Linux kernel module for real-time system monitoring
 * @author Mahmoud Ezzat
 * @date 2025-01-01
 * @version 1.0
 *
 * This module provides real-time monitoring capabilities for the Linux kernel,
 * exposing system metrics through the proc filesystem interface at
 * /proc/kernel_monitor. It collects and reports:
 * - CPU usage statistics
 * - Memory utilization
 * - Process information and memory consumption
 *
 * The module uses the seq_file interface for efficient data presentation
 * and follows modern kernel coding standards.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kernel_stat.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/sysinfo.h>

/** proc filesystem entry name */
#define PROC_NAME "kernel_monitor"

/** Module version information */
#define MODULE_VERSION "1.0.0"

/**
 * proc_show - Callback function to display kernel monitor data
 * @m: seq_file structure for output
 * @v: unused parameter (required by seq_file interface)
 *
 * This function is called when a user reads from /proc/kernel_monitor.
 * It gathers system statistics and formats them for display.
 *
 * Return: 0 on success, negative error code on failure
 */
static int proc_show(struct seq_file *m, void *v)
{
    struct task_struct *task;
    struct mm_struct *mm;
    struct sysinfo mem_info;
    u64 user_time, system_time, idle_time;
    unsigned long total_processes = 0;

    /* Print header */
    seq_printf(m, "===========================================\n");
    seq_printf(m, "     Linux Kernel Monitor v%s\n", MODULE_VERSION);
    seq_printf(m, "===========================================\n\n");

    /* Collect and display CPU statistics */
    user_time = kcpustat_cpu(0).cpustat[CPUTIME_USER];
    system_time = kcpustat_cpu(0).cpustat[CPUTIME_SYSTEM];
    idle_time = kcpustat_cpu(0).cpustat[CPUTIME_IDLE];
    
    seq_printf(m, "CPU Statistics (CPU 0):\n");
    seq_printf(m, "  User Time:   %llu ns\n", user_time);
    seq_printf(m, "  System Time: %llu ns\n", system_time);
    seq_printf(m, "  Idle Time:   %llu ns\n\n", idle_time);

    /* Collect and display memory statistics */
    si_meminfo(&mem_info);
    seq_printf(m, "Memory Statistics:\n");
    seq_printf(m, "  Total RAM:   %lu pages (%lu MB)\n", 
               mem_info.totalram, (mem_info.totalram * 4) / 1024);
    seq_printf(m, "  Free RAM:    %lu pages (%lu MB)\n", 
               mem_info.freeram, (mem_info.freeram * 4) / 1024);
    seq_printf(m, "  Shared RAM:  %lu pages\n", mem_info.sharedram);
    seq_printf(m, "  Buffer RAM:  %lu pages\n\n", mem_info.bufferram);

    /* Display process information */
    seq_printf(m, "Process Information:\n");
    seq_printf(m, "%-20s %-8s %-12s\n", "Name", "PID", "Memory (KB)");
    seq_printf(m, "-------------------------------------------\n");

    /* Iterate through all processes */
    for_each_process(task) {
        mm = get_task_mm(task);
        if (mm) {
            seq_printf(m, "%-20s %-8d %-12lu\n", 
                       task->comm, 
                       task->pid,
                       (mm->total_vm * 4)); /* Convert pages to KB */
            mmput(mm);  /* Release reference to mm_struct */
            total_processes++;
        }
    }

    seq_printf(m, "\nTotal Processes: %lu\n", total_processes);

    return 0;
}

/**
 * proc_open - Open callback for proc file
 * @inode: inode structure
 * @file: file structure
 *
 * Opens the proc file and associates it with the seq_file interface.
 *
 * Return: 0 on success, negative error code on failure
 */
static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}

/**
 * proc file operations structure
 * Defines callbacks for proc file operations
 */
static const struct proc_ops proc_fops = {
    .proc_open    = proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

/**
 * kernel_monitor_init - Module initialization function
 *
 * Creates the proc filesystem entry when the module is loaded.
 *
 * Return: 0 on success, -ENOMEM if proc entry creation fails
 */
static int __init kernel_monitor_init(void)
{
    struct proc_dir_entry *entry;

    /* Create proc entry with read permissions for all users */
    entry = proc_create(PROC_NAME, 0444, NULL, &proc_fops);
    if (!entry) {
        pr_err("Kernel Monitor: Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    pr_info("Kernel Monitor: Module loaded successfully\n");
    pr_info("Kernel Monitor: Data available at /proc/%s\n", PROC_NAME);
    
    return 0;
}

/**
 * kernel_monitor_exit - Module cleanup function
 *
 * Removes the proc filesystem entry when the module is unloaded.
 */
static void __exit kernel_monitor_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    pr_info("Kernel Monitor: Module unloaded successfully\n");
}

/* Register module initialization and cleanup functions */
module_init(kernel_monitor_init);
module_exit(kernel_monitor_exit);

/* Module metadata */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mahmoud Ezzat <your.email@example.com>");
MODULE_DESCRIPTION("Real-time Linux kernel monitoring module");
MODULE_VERSION(MODULE_VERSION);
