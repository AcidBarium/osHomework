#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t customer;

int customers_waiting = 0;
int total_customers = 5;

void* barber(void* arg) {
    while (1) {
        sem_wait(&customer); // 没顾客就阻塞
        pthread_mutex_lock(&mutex);

        customers_waiting--;
        printf("Barber: Cutting hair... Customers waiting: %d\n", customers_waiting);
        total_customers--;

        pthread_mutex_unlock(&mutex);

        sleep(rand() % 2 + 1);

        if (total_customers == 0) {
            printf("Barber: All customers done, going to sleep.\n");
            break;
        }
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
    pthread_t barber_tid, customer_tids[5];
    srand(time(0));

    sem_init(&customer, 0, 0);

    pthread_create(&barber_tid, NULL, barber, NULL);

    for (int i = 0; i < 5; i++) {
        sleep(rand() % 3);
        pthread_create(&customer_tids[i], NULL, client, NULL);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(customer_tids[i], NULL);
    }

    pthread_join(barber_tid, NULL);

    sem_destroy(&customer);
    return 0;
}
