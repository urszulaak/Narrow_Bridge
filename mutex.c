#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

pthread_mutex_t car_mutex;
pthread_mutex_t cityA_mutex;
pthread_mutex_t cityB_mutex;
pthread_mutex_t queueA_mutex;
pthread_mutex_t queueB_mutex;
int cityA, cityB, queueA, queueB, bridge;


void* car(void* arg) {
    int id = *(int*)arg;
    char direction[3];
    int type = rand() % 2; // Losowe miasto początkowe
    while (1) {
        int wait = (rand() % 5) + 1;
        if (!type) { // miasto A
            printf("Car %d stay in city A for %d seconds\n", id, wait);
            pthread_mutex_lock(&cityA_mutex);
            cityA++;
            pthread_mutex_unlock(&cityA_mutex);
            sleep(wait);

            printf("Car %d from city A wants to cross the bridge\n", id);
            pthread_mutex_lock(&queueA_mutex);
            cityA--;
            queueA++;
            pthread_mutex_unlock(&queueA_mutex);

            pthread_mutex_lock(&car_mutex); // wjazd na most
            sprintf(direction, ">>");
            queueA--;
            printf("A-%d %d --> [%s %d %s] <-- %d %d-B\n", cityA, queueA, direction, id, direction, queueB, cityB);
            sleep(1);
            pthread_mutex_unlock(&car_mutex);

            pthread_mutex_lock(&cityB_mutex);
            cityB++;
            type = 1;
            pthread_mutex_unlock(&cityB_mutex);

        } else { // miasto B
            printf("Car %d stay in city B for %d seconds\n", id, wait);
            pthread_mutex_lock(&cityB_mutex);
            cityB++;
            pthread_mutex_unlock(&cityB_mutex);
            sleep(wait);

            printf("Car %d from city B wants to cross the bridge\n", id);
            pthread_mutex_lock(&queueB_mutex);
            cityB--;
            queueB++;
            pthread_mutex_unlock(&queueB_mutex);

            pthread_mutex_lock(&car_mutex); // wjazd na most
            sprintf(direction, "<<");
            queueB--;
            printf("A-%d %d --> [%s %d %s] <-- %d %d-B\n", cityA, queueA, direction, id, direction, queueB, cityB);
            sleep(1);
            pthread_mutex_unlock(&car_mutex);

            pthread_mutex_lock(&cityA_mutex);
            cityA++;
            type = 0;
            pthread_mutex_unlock(&cityA_mutex);
        }

        // Po przejeździe przez most, samochód zmienia miasto
        sprintf(direction, "");
    }
    return NULL;
}

int mutex(int N, int info){
    pthread_t cars[N];
    int ids[N];
    srand(time(NULL));
    pthread_mutex_init(&car_mutex, NULL);
    pthread_mutex_init(&cityA_mutex, NULL);
    pthread_mutex_init(&cityB_mutex, NULL);
    pthread_mutex_init(&queueA_mutex, NULL);
    pthread_mutex_init(&queueB_mutex, NULL);
    for(int i=0;i<N;i++){
        ids[i] = i + 1;
        if(pthread_create(&cars[i], NULL, &car, &ids[i])!=0){
            return -1;
        }
    }
    for(int i=0;i<N;i++){
        if(pthread_join(cars[i], NULL)!=0){
            return -1;
        }
    }
    pthread_mutex_destroy(&car_mutex);
    return 0;
}