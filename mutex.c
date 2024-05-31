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

void* car(void* arg){
    //int wait = (rand()%10)+1; //ile czeka w miescie
    int type = (rand()%2); //z jakiego miasta startuje 0-miasto_A 1-miasto_B
    int id = *(int*)arg; //id samochodu
    char direction[3];
    while(1){
        int wait = (rand()%10)+1;
        if(type){
            printf("Car %d stay in city A for %d\n",id,wait);
            pthread_mutex_lock(&cityA_mutex);
            cityA++;
            pthread_mutex_unlock(&cityA_mutex);
        }else{
            printf("Car %d stay in city B for %d\n",id,wait);
            pthread_mutex_lock(&cityB_mutex);
            cityB++;
            pthread_mutex_unlock(&cityB_mutex);
        }
        if(strncmp(direction,">>",2) == 0 || strncmp(direction,"<<",2) == 0){
            printf("A-%d %d --> [%s %d %s] <-- %d %d-B\n",cityA,queueA,direction,id,direction,queueB,cityB);
        }else{
            printf("A-%d %d --> [] <-- %d %d-B\n",cityA,queueA,queueB,cityB);
        }
        sleep(wait);
        if(type){
            printf("Car %d from city A want cross the bridge\n",id);
            pthread_mutex_lock(&queueA_mutex);
            cityA--;
            queueA++;
            pthread_mutex_unlock(&queueA_mutex);
        }else{
            printf("Car %d from city B want cross the bridge\n",id);
            pthread_mutex_lock(&queueB_mutex);
            cityB--;
            queueB++;
            pthread_mutex_unlock(&queueB_mutex);
        }
        if(strncmp(direction,">>",2) == 0 || strncmp(direction,"<<",2) == 0){
            printf("A-%d %d --> [%s %d %s] <-- %d %d-B\n",cityA,queueA,direction,id,direction,queueB,cityB);
        }else{
            printf("A-%d %d --> [] <-- %d %d-B\n",cityA,queueA,queueB,cityB);
        }
        pthread_mutex_lock(&car_mutex);
        if(type){
            sprintf(direction,"%s",">>");
            queueA--;
        }else{
            sprintf(direction,"%s","<<");
            queueB--;
        }
        if(strncmp(direction,">>",2) == 0 || strncmp(direction,"<<",2) == 0){
            printf("A-%d %d --> [%s %d %s] <-- %d %d-B\n",cityA,queueA,direction,id,direction,queueB,cityB);
        }else{
            printf("A-%d %d --> [] <-- %d %d-B\n",cityA,queueA,queueB,cityB);
        }
        sprintf(direction,"%s","");
        if(type){
            type=0;
        }else{
            type=1;
        }
        pthread_mutex_unlock(&car_mutex);
    }
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