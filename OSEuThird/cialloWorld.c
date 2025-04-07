#include <pthread.h>
#include <stdio.h>

void *worker(void *arg)
{
    printf("Ciallo world!\n");
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t tid;

    if (pthread_create(&tid, NULL, worker, NULL))
    {
        printf("can not create\n");
        return 1;
    }

    printf("main waiting for thread\n");

    pthread_join(tid, NULL);

    return 0;
}
