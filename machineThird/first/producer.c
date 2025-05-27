/*
	Filename: producer.c
	Copyright: (C) 2006 by zhonghonglie
	Function: 模拟生产者进程，向共享缓冲区写入数据
*/
#include "ipc.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
	int rate;
	// 通过命令行参数设置生产速率（默认3秒）
	if (argv[1] != NULL)
		rate = atoi(argv[1]);
	else
		rate = 3;

	// 共享内存相关配置
	buff_key = 101;     // 缓冲区共享内存键值
	buff_num = 8;       // 缓冲区大小（字节数）
	pput_key = 102;     // 生产者指针键值
	pput_num = 1;       // 指针数量
	shm_flg = IPC_CREAT | 0644; // 共享内存权限（读写）

	// 获取/创建共享内存（缓冲区和指针）
	buff_ptr = (char *)set_shm(buff_key, buff_num, shm_flg); // 缓冲区首地址
	pput_ptr = (int *)set_shm(pput_key, pput_num, shm_flg);  // 写指针地址

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
	pmtx_sem = set_sem(pmtx_key, sem_val, sem_flg); // 创建生产者互斥信号量

	// 持续生产数据
	while (1) {
		down(prod_sem);  // 等待空槽位（P操作）
		down(pmtx_sem);  // 进入临界区（P操作）

		// 模拟生产数据：写入字符（A+位置索引）
		buff_ptr[*pput_ptr] = 'A' + (*pput_ptr % 26); // 防止溢出ASCII范围
		sleep(rate); // 模拟生产耗时
		printf("%d producer put: %c to Buffer[%d]\n", getpid(), buff_ptr[*pput_ptr], *pput_ptr);

		// 更新写指针位置（循环缓冲区）
		*pput_ptr = (*pput_ptr + 1) % buff_num;

		up(pmtx_sem);  // 离开临界区（V操作）
		up(cons_sem);  // 通知消费者有新产品（V操作）
	}

	return EXIT_SUCCESS;
}