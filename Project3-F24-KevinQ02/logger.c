#include "logger.h"
#include <stdio.h>
#include <time.h>

void log_event(const char* message) {
    FILE* log_file = fopen("log.txt", "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        return;
    }
    time_t now = time(NULL);
    fprintf(log_file, "%s: %s\n", ctime(&now), message);
    fclose(log_file);
}