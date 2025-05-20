#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int pid;
    int pipe1[2];
    int pipe2[2];

    int x;

    if (pipe(pipe1) < 0) {
        perror("failed to create pipe1");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipe2) < 0) {
        perror("failed to create pipe2");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("failed to create new process");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // 子进程=>父进程：子进程通过pipe2[1]写
        // 子进程<=父进程：子进程通过pipe1[0]读
        // 因此，在子进程中将pipe1[1]和pipe2[0]关闭
        close(pipe1[1]);
        close(pipe2[0]);

        do {
            read(pipe1[0], &x, sizeof(int));
            printf("child %d read: %d\n", getpid(), x++);
            write(pipe2[1], &x, sizeof(int));
        } while (x <= 9);

        close(pipe1[0]);
        close(pipe2[1]);
    } else {
        // 父进程<=子进程: 父进程从pipe2[0]读取子进程传过来的数
        // 父进程=>子进程: 父进程将更新的值通过pipe1[1]写入，传给子进程
        // 因此，父进程会先关闭pipe1[0]和pipe2[1]端口
        close(pipe1[0]);
        close(pipe2[1]);

        x = 1;

        do {
            write(pipe1[1], &x, sizeof(int));
            read(pipe2[0], &x, sizeof(int));
            printf("parent %d read: %d\n", getpid(), x++);
        } while (x <= 9);

        close(pipe1[1]);
        close(pipe2[0]);
    }

    return EXIT_SUCCESS;
}
