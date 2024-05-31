#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

pthread_mutex_t car_mutex;

void* car(void* arg){
    int wait = (rand()%10)+1; //ile czeka w miescie
    int type = (rand()%2); //z jakiego miasta startuje 0-miasto_A 1-miasto_B
    int id = *(int*)arg;
    if(type){
        printf("Car %d stay in city A\n",id);
    }else{
        printf("Car %d stay in city B\n",id);
    }  
}

int mutex(int N, int info){
    pthread_t cars[N];
    int ids[N];
    srand(time(NULL));
        pthread_mutex_init(&car_mutex, NULL);
    for(int i=1;i<N+1;i++){
        if(pthread_create(&cars[i], NULL, &car, &ids[1])!=0){
            return -1;
        }
    }
    for(int i=1;i<N+1;i++){
        if(pthread_join(cars[i], NULL)!=0){
            return -1;
        }
    }
    return 0;
}