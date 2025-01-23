#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"
#include "logger.h"

void decompress_source_file(const char* source, const char* destination) {
    char command[256];
    sprintf(command, "mkdir -p \"%s\" && tar --strip-components=1 -xzf \"%s\" -C \"%s\"", destination, source, destination);
    int result = system(command);
    if (result != 0) {
        printf("Error: Decompression failed\n");
    } else {
        printf("Decompression successful to %s\n", destination);
    }
    // List files in unpacked_files to confirm extraction
    sprintf(command, "ls -l \"%s\"", destination);
    system(command);
}

void create_pipes(int num_workers, int pipes[][2]) {
    for (int i = 0; i < num_workers; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Failed to create pipe");
            exit(EXIT_FAILURE);
        }
    }
}

void create_workers(int num_workers, int pipes[][2], pid_t* workers) {
    for (int i = 0; i < num_workers; i++) {
        workers[i] = fork();
        if (workers[i] == 0) {  // Child process
            close(pipes[i][1]); // Close unused write end
            worker_process(pipes[i][0], pipes[i][1]);
            exit(0);
        } else {
            close(pipes[i][0]); // Close unused read end in parent
        }
    }
}

void worker_process(int read_fd, int write_fd) {
    char filepath[1024];
    while (1) {
        ssize_t bytes_read = read(read_fd, filepath, sizeof(filepath));
        
        if (bytes_read <= 0) {
            // Pipe closed or read error
            printf("Worker received shutdown or read failed. Exiting...\n");
            break;
        }
        // Check for shutdown command
        if (strcmp(filepath, "SHUTDOWN") == 0) {
            printf("Worker received shutdown signal. Exiting...\n");
            break;  // Exit loop on shutdown signal
        }
        // Process the file
        printf("Worker received file to compress: %s\n", filepath);
        compress_file(filepath, write_fd);
    }

    // Close file descriptors
    close(read_fd);
    close(write_fd);
    printf("Worker terminated.\n");
}

void compress_file(const char* filepath, int write_fd) {
    char output_path[1024];
    sprintf(output_path, "%s.gz", filepath);
    char command[1024];
    sprintf(command, "gzip -c \"%s\" > \"%s\"", filepath, output_path);
    int result = system(command);  // Captures the return value of the system call

    if (result != 0) {
        printf("Error compressing file: %s\n", filepath);
    } else {
        printf("File compressed successfully: %s\n", output_path);
    }

    // Send the result back to the main process
    write(write_fd, &result, sizeof(result));
}

void cleanup_workers(int num_workers, pid_t* workers) {
    for (int i = 0; i < num_workers; i++) {
        printf("Waiting for worker %d to finish...\n", i);
        int status;
        waitpid(workers[i], &status, 0);
        if (WIFEXITED(status)) {
            printf("Worker %d has exited with status %d.\n", i, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Worker %d was terminated by signal %d.\n", i, WTERMSIG(status));
        }
    }
    printf("All workers have been cleaned up.\n");
}




void send_shutdown_signals(int num_workers, int pipes[][2]) {
    for (int i = 0; i < num_workers; i++) {
        printf("Sending shutdown signal to worker %d\n", i);
        write(pipes[i][1], "SHUTDOWN", strlen("SHUTDOWN") + 1);
        close(pipes[i][1]);  // Close the write end to signal EOF to the worker
    }
    printf("All shutdown signals sent.\n");
}




void collect_progress_reports(int num_workers, int pipes[][2]) {
    int result;
    for (int i = 0; i < num_workers; i++) {
        while (read(pipes[i][0], &result, sizeof(result)) > 0) {
            if (result == 0) {
                log_event("File compressed successfully.");
            } else {
                log_event("File compression failed.");
            }
        }
        close(pipes[i][0]);  // Close read end after finishing
    }
    printf("Progress reports collected.\n");
}

