# Git 使用与多线程编程实验报告

## 1. GitHub 仓库创建与管理

为了实现本地与虚拟机之间的协同开发，本实验首先创建了一个 GitHub 仓库，并将实验程序和记录上传至该仓库，便于在不同环境中同步和管理代码版本。

如图所示:

![alt text](img/1.png)

## 2. SSH 密钥配置与 GitHub 免密登录

使用下面的命令来生成ssh密钥，并将邮箱 acidbarium@163.com 作为密钥的注释，方便​GitHub免密登录。

```bash
ssh-keygen -t ed25519 -C "acidbarium@163.com"
```

如图所示

![alt text](img/2.png)

使用下面的命令来查看SSH公钥​​

```bash
cat~/.ssh/id_ed25519.pub
```

将该公钥添加至 GitHub 中的 SSH 密钥设置，如图所示：

![alt text](img/3.png)

然后，使用以下命令来验证 SSH 密钥是否已正确配置并能成功连接 GitHub：

```bash
ssh -T git@github.com
```

如图所示，配置正确，显示成功消息：

![alt text](img/5.png)

## 3. Linux开发环境配置

为进行代码开发，首先在Linux上创建一个新的文件夹，如图所示

![alt text](img/6.png)

通过下面的命令下载git

```bash
sudo yum install -y git
```

![alt text](img/7.png)


通过 Git 克隆仓库到Linux：

```bash
git clone git@github.com:AcidBarium/osHomework.git
```

如图所示

![alt text](img/8.png)

![alt text](img/9.png)


在仓库管理过程中，使用以下命令同步更新代码并提交更改：

```bash
git pull origin main
git add .
git commit -m "提交信息"
git push origin main
```


## 4. 多线程编程实验

### 4.1 基本多线程程序设计

首先编写一个简单的多线程程序，该程序使用 `pthread` 库创建一个线程并输出 "Ciallo world!"。以下是代码示例：

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

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
        exit(1);
    }

    printf("main waiting for thread\n");

    pthread_join(tid, NULL);

    exit(0);
}
```

如下图所示

![alt text](img/11.png)

通过下面的指令编译

```bash
gcc cialloWorld.c -o cialloWorld -pthread
```

运行结果如下
```
main waiting for thread
Ciallo world!
```
如下图所示

![alt text](img/12.png)


### 4.2 多线程性能测试

为了测试多线程在大规模计算中的性能提升，编写了一个多线程计算和单线程计算对比的程序。该程序计算从 1 到 `MAX_NUM`（10000000）的整数之和，并使用2个线程分配计算任务，从而提高计算效率。

编写代码如下

```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>

#define MAX_NUM 10000000
#define THREADS 2

// 线程参数结构体
typedef struct
{
    uint64_t start;
    uint64_t end;
    uint64_t result;
} ThreadArgs;

// 线程函数：计算 [start, end] 的和
void *calculate_sum(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    uint64_t sum = 0;
    for (uint64_t i = args->start; i <= args->end; i++)
    {
        sum += i;
    }
    args->result = sum;
    return NULL;
}

// 单线程计算
uint64_t single_thread_sum()
{
    uint64_t sum = 0;
    for (uint64_t i = 1; i <= MAX_NUM; i++)
    {
        sum += i;
    }
    return sum;
}

// 多线程计算
uint64_t multi_thread_sum()
{
    pthread_t threads[THREADS];
    ThreadArgs args[THREADS];
    uint64_t total_sum = 0;

    // 分配计算范围
    uint64_t segment = MAX_NUM / THREADS;
    for (int i = 0; i < THREADS; i++)
    {
        args[i].start = i * segment + 1;
        args[i].end = (i == THREADS - 1) ? MAX_NUM : (i + 1) * segment;
        args[i].result = 0;
        pthread_create(&threads[i], NULL, calculate_sum, &args[i]);
    }

    // 等待所有线程完成
    for (int i = 0; i < THREADS; i++)
    {
        pthread_join(threads[i], NULL);
        total_sum += args[i].result;
    }

    return total_sum;
}

int main()
{
    clock_t start, end;
    uint64_t sum;
    double time_used;

    // 单线程测试
    start = clock();
    sum = single_thread_sum();
    end = clock();
    time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("单线程结果: %lu, 耗时: %.5f 秒\n", sum, time_used);

    // 多线程测试
    start = clock();
    sum = multi_thread_sum();
    end = clock();
    time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("多线程结果: %lu, 耗时: %.5f 秒\n", sum, time_used);

    return 0;
}
```

运行结果如下图所示

![alt text](img/13.png)

运行该程序后，可以观察到多线程计算的时间明显低于单线程计算，在多线程的支持下，程序的计算速度大约提高了一倍，证明了多线程在并行计算中的优势。


