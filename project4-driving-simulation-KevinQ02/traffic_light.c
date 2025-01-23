#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include "traffic_light.h"
#include "constants.h"

// External declarations
extern pthread_mutex_t road_mutexes[];
extern pthread_cond_t traffic_light_conditions[];
extern int traffic_light_status[];

// Traffic light thread logic function
void* traffic_light_thread_logic(void* arg) {
    int road_id = *((int*)arg);
    free(arg);

    while (1) {
        // Simulate the green light
        pthread_mutex_lock(&road_mutexes[road_id]);
        traffic_light_status[road_id] = 1; // Set light to GREEN
        printf("Traffic light on road %d is GREEN. %ld\n", road_id, time(NULL));
        pthread_cond_broadcast(&traffic_light_conditions[road_id]);
        pthread_mutex_unlock(&road_mutexes[road_id]);

        sleep(rand() % 6 + 5); // Random green duration between 5-10 seconds

        // Simulate the red light
        pthread_mutex_lock(&road_mutexes[road_id]);
        traffic_light_status[road_id] = 0; // Set light to RED
        printf("Traffic light on road %d is RED. %ld\n", road_id, time(NULL));
        pthread_mutex_unlock(&road_mutexes[road_id]);

        sleep(rand() % 6 + 5); // Random red duration between 5-10 seconds
    }

    return NULL;
}
/**/