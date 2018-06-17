#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

int ticks=0;
int elapsed_time=0;
void sig_handler(int);

void* time_triggered(void);
pthread_cond_t cond_alarm = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int main(void) {
    
    pthread_t thread;

    struct itimerval delay;
    int ret;

    delay.it_value.tv_sec=0;
    delay.it_value.tv_usec=10;
    delay.it_interval.tv_sec=0;
    delay.it_interval.tv_usec=10000;
    signal(SIGALRM, sig_handler);

    pthread_create(&thread, NULL,(void *) time_triggered, NULL);
    ret = setitimer(ITIMER_REAL, &delay, NULL);

    
    
    while(1) {
        pause();
    }
}

void sig_handler(int signo) {
    ticks++;
    if (ticks%100 == 0) {
        ticks = 0;
        pthread_cond_signal(&cond_alarm);
    }
    signal(SIGALRM, sig_handler);
}

void* time_triggered(void) {
    
    while(1) {
        pthread_cond_wait(&cond_alarm, &mutex);
        elapsed_time++;
        printf("current time : %d\n", elapsed_time);
        if (elapsed_time > 5) exit(0);
    }
}