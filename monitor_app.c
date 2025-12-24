/**
 * @file monitor_app.c
 * @brief User-space application for Linux Kernel Monitor
 * @author Kholoud Diaa
 * @date 2025-01-01
 * @version 1.0
 *
 * This application reads and displays real-time system statistics
 * from the kernel monitor module via /proc/kernel_monitor.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>

/* Configuration constants */
#define PROC_PATH "/proc/kernel_monitor"
#define BUFFER_SIZE 4096
#define APP_VERSION "1.0.0"

/* Color codes for terminal output */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_BOLD    "\033[1m"

/**
 * print_usage - Display usage information
 * @prog_name: Name of the program
 */
static void print_usage(const char *prog_name)
{
    printf("Usage: %s [OPTIONS]\n", prog_name);
    printf("Read and display Linux kernel monitoring data\n\n");
    printf("Options:\n");
    printf("  -h, --help       Display this help message\n");
    printf("  -v, --version    Display version information\n");
    printf("  -r, --raw        Display raw output without formatting\n");
    printf("  -w, --watch SEC  Continuously display data every SEC seconds\n");
    printf("\nExamples:\n");
    printf("  %s              Display current system statistics\n", prog_name);
    printf("  %s -w 2         Update display every 2 seconds\n", prog_name);
}

/**
 * print_version - Display version information
 */
static void print_version(void)
{
    printf("Kernel Monitor Application v%s\n", APP_VERSION);
    printf("Copyright (C) 2025 Mahmoud Ezzat\n");
}

/**
 * read_kernel_data - Read data from kernel module
 * @buffer: Buffer to store read data
 * @size: Size of buffer
 *
 * Return: Number of bytes read on success, -1 on failure
 */
static ssize_t read_kernel_data(char *buffer, size_t size)
{
    int fd;
    ssize_t bytes_read;

    /* Open proc file */
    fd = open(PROC_PATH, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, COLOR_RED "Error: Failed to open %s: %s\n" COLOR_RESET,
                PROC_PATH, strerror(errno));
        fprintf(stderr, "Make sure the kernel module is loaded (insmod kernel_monitor.ko)\n");
        return -1;
    }

    /* Read data from proc file */
    bytes_read = read(fd, buffer, size - 1);
    if (bytes_read < 0) {
        fprintf(stderr, COLOR_RED "Error: Failed to read from %s: %s\n" COLOR_RESET,
                PROC_PATH, strerror(errno));
        close(fd);
        return -1;
    }

    /* Null-terminate the buffer */
    buffer[bytes_read] = '\0';
    close(fd);

    return bytes_read;
}

/**
 * display_data - Display kernel data with formatting
 * @raw: If true, display raw output; otherwise, add formatting
 */
static void display_data(int raw)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    /* Read data from kernel */
    bytes_read = read_kernel_data(buffer, BUFFER_SIZE);
    if (bytes_read < 0) {
        return;
    }

    /* Display data */
    if (raw) {
        printf("%s", buffer);
    } else {
        /* Clear screen for formatted output */
        printf("\033[2J\033[H");
        printf(COLOR_BOLD COLOR_BLUE);
        printf("╔════════════════════════════════════════════════════════╗\n");
        printf("║         Linux Kernel Monitor - Live View              ║\n");
        printf("╚════════════════════════════════════════════════════════╝\n");
        printf(COLOR_RESET);
        printf("\n%s\n", buffer);
    }
}

/**
 * watch_mode - Continuously display data at specified intervals
 * @interval: Time in seconds between updates
 */
static void watch_mode(int interval)
{
    printf(COLOR_GREEN "Starting watch mode (updating every %d seconds)...\n", interval);
    printf("Press Ctrl+C to exit\n" COLOR_RESET);
    sleep(2);

    while (1) {
        display_data(0);
        sleep(interval);
    }
}

/**
 * main - Entry point of the application
 * @argc: Argument count
 * @argv: Argument vector
 *
 * Return: EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int main(int argc, char *argv[])
{
    int opt;
    int raw_mode = 0;
    int watch_interval = 0;

    /* Define long options */
    static struct option long_options[] = {
        {"help",    no_argument,       0, 'h'},
        {"version", no_argument,       0, 'v'},
        {"raw",     no_argument,       0, 'r'},
        {"watch",   required_argument, 0, 'w'},
        {0, 0, 0, 0}
    };

    /* Parse command line options */
    while ((opt = getopt_long(argc, argv, "hvrw:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            case 'v':
                print_version();
                return EXIT_SUCCESS;
            case 'r':
                raw_mode = 1;
                break;
            case 'w':
                watch_interval = atoi(optarg);
                if (watch_interval <= 0) {
                    fprintf(stderr, "Error: Invalid watch interval\n");
                    return EXIT_FAILURE;
                }
                break;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    /* Execute based on mode */
    if (watch_interval > 0) {
        watch_mode(watch_interval);
    } else {
        display_data(raw_mode);
    }

    return EXIT_SUCCESS;
}
