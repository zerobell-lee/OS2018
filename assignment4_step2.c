#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_SIZE 10

int ticks=0;
int elapsed_time=0;
void sig_handler(int);

time_t t;
struct tm tm;

void tt_thread_register(int period, int thread_id);
void tt_thread_wait_invocation(int thread_id);

struct TCB {
    int period;
    int thread_id;
    int time_left_to_invoke;
};

void* time_triggered(void* arg);
pthread_cond_t cond_alarm[MAX_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct TCB TCB_array[MAX_SIZE];



int main(void) {
    
    
    time(&t);

    pthread_t thread[3];

    

    struct itimerval delay;
    int ret;

    delay.it_value.tv_sec=0;
    delay.it_value.tv_usec=10;
    delay.it_interval.tv_sec=0;
    delay.it_interval.tv_usec=1000;
    signal(SIGALRM, sig_handler);


    for (int i=0; i<10; i++)
        pthread_cond_init(&cond_alarm[i], NULL);

    for (int i=0; i<3; i++)
        pthread_create(&thread[i], NULL, (void *)time_triggered, (void *)i);

    ret = setitimer(ITIMER_REAL, &delay, NULL);

    while(1) {
        pause();
    }
}

void sig_handler(int signo) {
    pthread_mutex_lock(&mutex);
    for (int i=0; i<3; i++) {
        TCB_array[i].time_left_to_invoke -= 10;
        if (TCB_array[i].time_left_to_invoke <= 0) {
            TCB_array[i].time_left_to_invoke = TCB_array[i].period;
            pthread_cond_signal(&cond_alarm[i]);
        }
    }
    pthread_mutex_unlock(&mutex);
    signal(SIGALRM, sig_handler);
}

void* time_triggered(void* arg) {
    int id = (int) arg;
    tt_thread_register(id+1, id);
    while(1) {
        tt_thread_wait_invocation(id);
        time(&t);
        printf("id : %d - %s", id, ctime(&t));
    }
}

void tt_thread_register(int period, int thread_id) {
    pthread_mutex_lock(&mutex);
    TCB_array[thread_id].period = period*10000;
    TCB_array[thread_id].thread_id = thread_id;
    TCB_array[thread_id].time_left_to_invoke = period*10000;
    pthread_mutex_unlock(&mutex);
}
void tt_thread_wait_invocation(int thread_id) {
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond_alarm[thread_id], &mutex);
    pthread_mutex_unlock(&mutex);
}