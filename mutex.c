#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

pthread_mutex_t mutex;

void* car(){
    
}

int mutex(int N, char** version){
    pthread_t cars;
    srand(time(NULL));
    for(int i=1;i<N+1;i++){
        if(pthread_create(&cars, NULL, &car, NULL)!=0){
            return -1;
        }
    }
    for(int i=1;i<N+1;i++){
        if(pthread_join(cars, NULL)!=0){
            return -1;
        }
    }
    return 0;
}