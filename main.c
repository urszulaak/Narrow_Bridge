#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "mutex.h"
#include "condition.h"


int main(int argc, char** argv){
    char version; //wybor wersji programu m-mutexy c-condition var
    printf("Which version of program would you like to choose?\nType described letter:\nm - mutex\nc - condition variables\n");
    scanf("%c",&version);
    printf("%c",version);
    int N=argv[1]; //liczba samochodow(watkow)
    char information=argv[2]; //informacja o wpisanym parametrze -info
    int info;
    if(strcmp("-info",information)==0){
        info=1;
    }else{
        info=0;
    }
    if(strcmp("m",version)==0){
        mutex(N, info);
    }else if(strcmp("c",version)==0){
        condition(N, info);
    }else{
        perror("Invalid version");
        return -1;
    }
}