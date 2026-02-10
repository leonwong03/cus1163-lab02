#include "proc_reader.h"

int list_process_directories(void) {
    
    DIR *dir = opendir("/proc");

    if (dir == NULL) {

        perror("opendir(/proc) failed");
        return -1;

    }

    struct dirent *entry;
    int count = 0;

    printf("Process directories in /proc:\n");
    printf("%-8s %-20s\n", "PID", "Type");
    printf("%-8s %-20s\n", "---", "----");

    while ((entry = readdir(dir)) != NULL) {

        if (is_number(entry->d_name)) {

            printf("%-8s %-20s\n", entry->d_name, "Process");
            count++;

        }

    }

    if (closedir(dir) != 0) {

        perror("closedir(/proc) failed");
        return -1;

    }

    printf("\nTotal Process Directories Found: %d\n", count);
    return 0;

}

int read_process_info(const char* pid) {
    char filepath[256];

   if (pid == NULL || pid[0] == '\0') {

        fprintf(stderr, "Error: PID is empty\n");
        return -1;

   }

    snprintf(filepath, sizeof(filepath), "/proc/%s/status", pid);

    printf("\n--- Process Information for PID %s ---\n", pid);

    if(read_file_with_syscalls(filepath) != 0) {

        fprintf(stderr, "Error: Couldn't Read %s\n", filepath);
        return -1;

    }

    snprintf(filepath, sizeof(filepath), "/proc/%s/cmdline", pid);

    printf("\n--- Command Line ---\n");

    if (read_file_with_syscalls(filepath) != 0) {

        fprintf(stderr, "Error: Couldn't Read %s\n", filepath);
        return -1;

    }

    printf("\n"); 
    return 0; 

}

int show_system_info(void) {
    int line_count = 0;
    const int MAX_LINES = 10;

    FILE *fp = NULL;
    char line[512];

    printf("\n--- CPU Information (first %d lines) ---\n", MAX_LINES);

    fp = fopen("/proc/cpuinfo", "r");

    if (fp == NULL) {

        perror("fopen(/proc/cpuinfo) failed");
        return -1;

    }

    line_count = 0;
    while (line_count < MAX_LINES && fgets (line, sizeof(line), fp) != NULL) {

        printf("%s",line);
        line_count++;

    }

    if (fclose(fp) != 0) {

        perror("fclose(/proc/cpuinfo) failed");
        return -1;

    }

    printf("\n--- Memory Information (first %d lines) ---\n", MAX_LINES);

    fp = fopen("/proc/meminfo", "r");

    if (fp == NULL) {

        perror("fopen(/proc/meminfo) failed");
        return -1;

    }

    line_count = 0;
    while (line_count < MAX_LINES && fgets(line, sizeof(line), fp) != NULL) {

        printf("%s",line);
        line_count++;

    }

    if (fclose(fp) != 0) {

        perror("fclose(/proc/meminfo) failed");
        return -1;

    }

    return 0; 

}

void compare_file_methods(void) {

    const char* test_file = "/proc/version";

    printf("Comparing file reading methods for: %s\n\n", test_file);

    printf("=== Method 1: Using System Calls ===\n");
    read_file_with_syscalls(test_file);

    printf("\n=== Method 2: Using Library Functions ===\n");
    read_file_with_library(test_file);

    printf("\nNOTE: Run this program with strace to see the difference!\n");
    printf("Example: strace -e trace=openat,read,write,close ./lab2\n");

}

int read_file_with_syscalls(const char* filename) {

    int fd;
    ssize_t bytes_read;
    char buffer[1024];

    if (filename == NULL || filename[0] == '\0') {

        fprintf(stderr, "Error: File Is Empty\n");
        return -1;

    }

    fd = open(filename, O_RDONLY);

    if (fd == -1) {

        perror("open() failed");
        return -1;

    }

    while ((bytes_read = read(fd, buffer, (ssize_t) sizeof(buffer) - 1)) > 0) {

        for (ssize_t i = 0; i < bytes_read; i++) {

            if (buffer[i] == '\0') buffer[i] = ' ';

        }

        buffer [bytes_read] = '\0';
        printf("%s", buffer);

    }

    if (bytes_read == -1) {

        perror("read() failed");
        close(fd);
        return -1;

    }

    if (close(fd) == -1) {

        perror ("close() failed");
        return -1;

    }
   
    return 0;
}

int read_file_with_library(const char* filename) {

    FILE *fp;
    char buffer [1024];

    if (filename == NULL || filename[0] == '\0') {

        fprintf(stderr, "Error: Filename Is Empty\n");
        return -1;

    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {

        printf("%s", buffer);

    }

    if (fclose(fp) != 0) {

        perror("fclose() failed");
        return -1;

    }
   
    return 0; 

}

int is_number(const char* str) {
   
    if (str == NULL || str[0] == '\0') {

        return 0;

    }

    while (*str) {

        if (!isdigit((unsigned char)*str)) {

            return 0;

        }

        str++;

    }

    return 1; 

}