#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t customer;  // 顾客数量（信号量）
int customers_waiting = 0; // 等待的顾客数量

void* barber(void* arg) {
    while (1) {
        sem_wait(&customer); // 等待顾客来叫醒
        pthread_mutex_lock(&mutex);

        customers_waiting--;
        printf("Barber: Cutting hair... Customers waiting: %d\n", customers_waiting);

        pthread_mutex_unlock(&mutex);

        sleep(rand() % 2 + 1); // 理发时间
    }
    return NULL;
}

void* client(void* arg) {
    pthread_mutex_lock(&mutex);

    customers_waiting++;
    printf("Customer arrived! Customers waiting: %d\n", customers_waiting);

    pthread_mutex_unlock(&mutex);

    sem_post(&customer); // 通知理发师

    return NULL;
}

int main() {
    pthread_t barber_tid, customer_tids[40];
    srand(time(0));

    sem_init(&customer, 0, 0);

    pthread_create(&barber_tid, NULL, barber, NULL);

    for (int i = 0; i < 30; i++) {
        sleep(rand() % 3); // 顾客随机到来
        pthread_create(&customer_tids[i], NULL, client, NULL);
    }

    for (int i = 0; i < 30; i++) {
        pthread_join(customer_tids[i], NULL);
    }

    // barber 是无限循环，不用 join

    sem_destroy(&customer);
    return 0;
}
