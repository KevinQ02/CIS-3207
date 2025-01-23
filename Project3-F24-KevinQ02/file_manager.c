#include "file_manager.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void distribute_files(const char* directory, int num_workers, int pipes[][2]) {
    DIR* dir = opendir(directory);
    struct dirent* entry;
    int current_worker = 0;

    if (!dir) {
        perror("Failed to open directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // Check for regular file type
            // Skip files that are already compressed (.gz)
            if (strstr(entry->d_name, ".gz") != NULL) {
                continue;
            }

            char filepath[1024];
            sprintf(filepath, "%s/%s", directory, entry->d_name);
            printf("Distributing file: %s to worker %d\n", filepath, current_worker);
            write(pipes[current_worker][1], filepath, strlen(filepath) + 1);
            current_worker = (current_worker + 1) % num_workers;
        }
    }

    closedir(dir);
}

