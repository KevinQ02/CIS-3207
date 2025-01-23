#include <stdio.h>
#include <time.h>
#include "logger.h"

// Logging function
void log_event(const char* message, int id, int road_id) {
    time_t current_time = time(NULL);
    char* time_str = ctime(&current_time);
    time_str[strlen(time_str) - 1] = '\0';  // Remove newline character

    printf("[%s] %s: Car %d on road %d\n", time_str, message, id, road_id);
}
/**/