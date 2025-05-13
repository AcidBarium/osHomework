#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int sum = 0; 
int max_sum = 10; 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER; 

void* producer(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (sum >= max_sum) {
            pthread_cond_wait(&not_full, &mutex);
        }
        sum++;
        printf("Producer produced an item. Current sum: %d\n", sum);
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
        sleep(rand() % 2 + 1);
    }

    return NULL;
}

void* consumer(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (sum <= 0) {
            pthread_cond_wait(&not_empty, &mutex);
        }
        sum--;
        printf("Consumer consumed an item. Current sum: %d\n", sum);
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
        sleep(rand() % 2 + 1);
    }
    return NULL;
}

int main() {
    pthread_t producers[2], consumers[2];
    for (int i = 0; i < 2; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }
    sleep(10);

    return 0;
}