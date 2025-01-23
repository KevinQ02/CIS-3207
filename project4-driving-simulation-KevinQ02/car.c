#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "car.h"
#include "constants.h"

// External declarations
extern pthread_mutex_t road_mutexes[];
extern pthread_cond_t traffic_light_conditions[];
extern int cars_on_road[];
extern int traffic_light_status[];

// Car thread logic function
void* car_thread_logic(void* arg) {
    int car_id = *((int*)arg);
    free(arg);

    while (1) {
        int road_id = rand() % NUM_ROADS;

        pthread_mutex_lock(&road_mutexes[road_id]);
        while (cars_on_road[road_id] >= ROAD_CAPACITY || traffic_light_status[road_id] == 0) {
            // Wait if the road is full or the light is red
            pthread_cond_wait(&traffic_light_conditions[road_id], &road_mutexes[road_id]);
        }

        // Enter the road
        cars_on_road[road_id]++;
        printf("Car %d entered road %d. Cars on road: %d. %ld\n", car_id, road_id, cars_on_road[road_id], time(NULL));

        pthread_mutex_unlock(&road_mutexes[road_id]);

        // Simulate driving on the road for a random amount of time
        sleep(rand() % 5 + 1);

        // Leave the road
        pthread_mutex_lock(&road_mutexes[road_id]);
        cars_on_road[road_id]--;
        printf("Car %d left road %d. Cars on road: %d. %ld\n", car_id, road_id, cars_on_road[road_id], time(NULL));

        // Signal that space is available on the road
        pthread_cond_signal(&traffic_light_conditions[road_id]);
        pthread_mutex_unlock(&road_mutexes[road_id]);

        // Wait before trying again
        sleep(rand() % 5 + 1);
    }

    return NULL;
}
/**/