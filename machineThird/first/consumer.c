/*
	Filename: consumer.c
	Copyright: (C) 2006 by zhonghonglie
	Function: 模拟消费者进程，从共享缓冲区读取数据
*/
#include "ipc.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
	int rate;
	// 通过命令行参数设置消费速率（默认3秒）
	if (argv[1] != NULL)
		rate = atoi(argv[1]);
	else
		rate = 3;

	// 共享内存相关配置
	buff_key = 101;     // 缓冲区共享内存键值
	buff_num = 8;       // 缓冲区大小（字节数）
	cget_key = 103;     // 消费者指针键值
	cget_num = 1;       // 指针数量
	shm_flg = IPC_CREAT | 0644; // 共享内存权限（读写）

	// 获取/创建共享内存（缓冲区和指针）
	buff_ptr = (char *)set_shm(buff_key, buff_num, shm_flg); // 缓冲区首地址
	cget_ptr = (int *)set_shm(cget_key, cget_num, shm_flg);  // 读指针地址

	// 信号量相关配置
	prod_key = 201;     // 生产者同步信号量键值
	pmtx_key = 202;     // 生产者互斥信号量键值
	cons_key = 301;     // 消费者同步信号量键值
	cmtx_key = 302;     // 消费者互斥信号量键值
	sem_flg = IPC_CREAT | 0644; // 信号量权限（读写）

	// 初始化信号量
	sem_val = buff_num; // 生产者同步信号量初值=缓冲区大小（可用空槽位数）
	prod_sem = set_sem(prod_key, sem_val, sem_flg); // 创建生产者同步信号量

	sem_val = 0;        // 消费者同步信号量初值=0（初始无产品可取）
	cons_sem = set_sem(cons_key, sem_val, sem_flg); // 创建消费者同步信号量

	sem_val = 1;        // 互斥信号量初值=1（保护缓冲区访问）
	cmtx_sem = set_sem(cmtx_key, sem_val, sem_flg); // 创建消费者互斥信号量

	// 持续消费数据
	while (1) {
		down(cons_sem);  // 等待缓冲区有产品（P操作）
		down(cmtx_sem);  // 进入临界区（P操作）

		sleep(rate); // 模拟消费耗时
		printf("%d consumer get: %c from Buffer[%d]\n", getpid(), buff_ptr[*cget_ptr], *cget_ptr);

		// 更新读指针位置（循环缓冲区）
		*cget_ptr = (*cget_ptr + 1) % buff_num;

		up(cmtx_sem);  // 离开临界区（V操作）
		up(prod_sem);  // 通知生产者有空槽位（V操作）
	}

	return EXIT_SUCCESS;
}
