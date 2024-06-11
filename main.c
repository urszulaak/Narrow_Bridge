#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <semaphore.h>

pthread_mutex_t cityA_mutex;
pthread_mutex_t cityB_mutex;
sem_t bridge_sem;
pthread_mutex_t queueA_mutex;
pthread_mutex_t queueB_mutex;
int info=0, cityA=0, cityB=0, queueA=0, queueB=0, bridge=0;

typedef struct ticket_lock {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    unsigned long queue_head, queue_tail;
} ticket_lock_t;

#define TICKET_LOCK_INITIALIZER { PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, 0, 0 }

void ticket_lock(ticket_lock_t *ticket) {
    unsigned long queue_me;

    pthread_mutex_lock(&ticket->mutex);
    queue_me = ticket->queue_tail++;
    while (queue_me != ticket->queue_head) {
        pthread_cond_wait(&ticket->cond, &ticket->mutex);
    }
    pthread_mutex_unlock(&ticket->mutex);
}

void ticket_unlock(ticket_lock_t *ticket) {
    pthread_mutex_lock(&ticket->mutex);
    ticket->queue_head++;
    pthread_cond_broadcast(&ticket->cond);
    pthread_mutex_unlock(&ticket->mutex);
}

ticket_lock_t bridge_lock = TICKET_LOCK_INITIALIZER;

float x = 1.5f;
void do_fake_work(long iter) {
    for (long i = 0; i < iter; i++) {
        x *= sin(x) / atan(x) * tanh(x) * sqrt(x);
    }
    x = 1.5f;
}

void* car(void* arg) {
    int id = *(int*)arg;
    char direction[3];
    int type = rand() % 2; // Losowe miasto początkowe
    while (1) {
        int wait = (rand() % 5) + 1;
        if (!type) { // miasto A
            if(info){
                printf("Car %d stay in city A for %d seconds\n", id, wait);
            }
            pthread_mutex_lock(&cityA_mutex);
            cityA++;
            pthread_mutex_unlock(&cityA_mutex);
            sleep(wait);
            if(info){
                printf("Car %d from city A wants to cross the bridge\n", id);
            }
            pthread_mutex_lock(&queueA_mutex);
            cityA--;
            queueA++;
            pthread_mutex_unlock(&queueA_mutex);
            sem_wait(&bridge_sem); // Wjazd na most
            sprintf(direction, ">>");
            queueA--;
            do_fake_work(100000);
            printf("A-%d %d --> [%s %d %s] <-- %d %d-B\n", cityA, queueA, direction, id, direction, queueB, cityB);
            type = 1;
            sem_post(&bridge_sem);
        } else { // miasto B
            if(info){
                printf("Car %d stay in city B for %d seconds\n", id, wait);
            }
            pthread_mutex_lock(&cityB_mutex);
            cityB++;
            pthread_mutex_unlock(&cityB_mutex);
            sleep(wait);
            if(info){
                printf("Car %d from city B wants to cross the bridge\n", id);
            }
            pthread_mutex_lock(&queueB_mutex);
            cityB--;
            queueB++;
            pthread_mutex_unlock(&queueB_mutex);
            sem_wait(&bridge_sem);
            sprintf(direction, "<<");
            queueB--;
            do_fake_work(100000);
            printf("A-%d %d --> [%s %d %s] <-- %d %d-B\n", cityA, queueA, direction, id, direction, queueB, cityB);
            type = 0;
            sem_post(&bridge_sem);
        }
        sprintf(direction, "");
    }
    return NULL;
}

void* car2(void* arg) {
    int id = *(int*)arg;
    char direction[3];
    int type = rand() % 2;
    while (1) {
        int wait = (rand() % 5) + 1;
        if (!type) {
            if(info){
                printf("Car %d stay in city A for %d seconds\n", id, wait);
            }
            pthread_mutex_lock(&cityA_mutex);
            cityA++;
            pthread_mutex_unlock(&cityA_mutex);
            sleep(wait);
            if(info){
                printf("Car %d from city A wants to cross the bridge\n", id);
            }
            pthread_mutex_lock(&queueA_mutex);
            cityA--;
            queueA++;
            pthread_mutex_unlock(&queueA_mutex);

            ticket_lock(&bridge_lock);
            queueA--;
            sprintf(direction, ">>");
            do_fake_work(100000);
            printf("A-%d %d --> [%s %d %s] <-- %d %d-B\n", cityA, queueA, direction, id, direction, queueB, cityB);
            type = 1;
            ticket_unlock(&bridge_lock);
        } else {
            if(info){
                printf("Car %d stay in city B for %d seconds\n", id, wait);
            }
            pthread_mutex_lock(&cityB_mutex);
            cityB++;
            pthread_mutex_unlock(&cityB_mutex);
            sleep(wait);
            if(info){
                printf("Car %d from city B wants to cross the bridge\n", id);
            }
            pthread_mutex_lock(&queueB_mutex);
            cityB--;
            queueB++;
            pthread_mutex_unlock(&queueB_mutex);

            ticket_lock(&bridge_lock);
            queueB--;
            sprintf(direction, "<<");
            do_fake_work(100000);
            printf("A-%d %d --> [%s %d %s] <-- %d %d-B\n", cityA, queueA, direction, id, direction, queueB, cityB);
            type = 0;
            ticket_unlock(&bridge_lock);
        }
        sprintf(direction, "");
    }
    return NULL;
}

int mutex(int N) {
    pthread_t cars[N];
    int ids[N];
    srand(time(NULL));
    pthread_mutex_init(&cityA_mutex, NULL);
    pthread_mutex_init(&cityB_mutex, NULL);
    pthread_mutex_init(&queueA_mutex, NULL);
    pthread_mutex_init(&queueB_mutex, NULL);
    sem_init(&bridge_sem, 0, 1);
    for (int i = 0; i < N; i++) {
        ids[i] = i + 1;
        if (pthread_create(&cars[i], NULL, &car, &ids[i]) != 0) {
            return -1;
        }
    }
    for (int i = 0; i < N; i++) {
        if (pthread_join(cars[i], NULL) != 0) {
            return -1;
        }
    }
    pthread_mutex_destroy(&cityA_mutex);
    pthread_mutex_destroy(&cityB_mutex);
    pthread_mutex_destroy(&queueA_mutex);
    pthread_mutex_destroy(&queueB_mutex);
    sem_destroy(&bridge_sem);
    return 0;
}

int condition(int N) {
    pthread_t cars[N];
    int ids[N];
    srand(time(NULL));
    pthread_mutex_init(&cityA_mutex, NULL);
    pthread_mutex_init(&cityB_mutex, NULL);
    pthread_mutex_init(&queueA_mutex, NULL);
    pthread_mutex_init(&queueB_mutex, NULL);
    for (int i = 0; i < N; i++) {
        ids[i] = i + 1;
        if (pthread_create(&cars[i], NULL, &car2, &ids[i]) != 0) {
            return -1;
        }
    }
    for (int i = 0; i < N; i++) {
        if (pthread_join(cars[i], NULL) != 0) {
            return -1;
        }
    }
    pthread_mutex_destroy(&cityA_mutex);
    pthread_mutex_destroy(&cityB_mutex);
    pthread_mutex_destroy(&queueA_mutex);
    pthread_mutex_destroy(&queueB_mutex);
    return 0;
}

int main(int argc, char** argv){
    char version; //wybor wersji programu m-mutexy c-condition var
    printf("Which version of program would you like to choose?\nType described letter:\nm - mutex\nc - condition variables\n");
    scanf("%c",&version);
    int N=atoi(argv[1]); //liczba samochodow(watkow)
    if(argc==3){
        char* information=argv[2]; //informacja o wpisanym parametrze -info
        if(strncmp("-info",information,5)==0){
            info=1;
        }else{
            info=0;
        }
    }
    if(strncmp("m",&version,1)==0){
        mutex(N);
    }else if(strncmp("c",&version,1)==0){
        condition(N);
    }else{
        perror("Invalid version");
        return -1;
    }
}
