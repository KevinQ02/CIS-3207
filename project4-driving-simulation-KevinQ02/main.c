#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "traffic_light.h"
#include "car.h"
#include "constants.h"

// Shared resources
pthread_mutex_t road_mutexes[NUM_ROADS];
pthread_cond_t traffic_light_conditions[NUM_ROADS];
int cars_on_road[NUM_ROADS] = {0};
int traffic_light_status[NUM_ROADS] = {0}; // 1 for GREEN, 0 for RED

int main() {
    srand(time(NULL));

    // Initialize mutexes and condition variables
    for (int i = 0; i < NUM_ROADS; i++) {
        pthread_mutex_init(&road_mutexes[i], NULL);
        pthread_cond_init(&traffic_light_conditions[i], NULL);
    }

    // Create traffic light threads
    pthread_t traffic_light_threads[NUM_ROADS];
    for (int i = 0; i < NUM_ROADS; i++) {
        int* road_id = malloc(sizeof(int));
        *road_id = i;
        pthread_create(&traffic_light_threads[i], NULL, traffic_light_thread_logic, road_id);
    }

    // Create car threads
    pthread_t car_threads[NUM_CARS];
    for (int i = 0; i < NUM_CARS; i++) {
        int* car_id = malloc(sizeof(int));
        *car_id = i;
        pthread_create(&car_threads[i], NULL, car_thread_logic, car_id);
    }

    // Join traffic light threads (this will block forever)
    for (int i = 0; i < NUM_ROADS; i++) {
        pthread_join(traffic_light_threads[i], NULL);
    }

    // Join car threads (in practice, car threads won't end in this simulation)
    for (int i = 0; i < NUM_CARS; i++) {
        pthread_join(car_threads[i], NULL);
    }

    // Cleanup
    for (int i = 0; i < NUM_ROADS; i++) {
        pthread_mutex_destroy(&road_mutexes[i]);
        pthread_cond_destroy(&traffic_light_conditions[i]);
    }

    return 0;
}
/**/