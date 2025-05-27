/*
	Filename: ipc.h
	Copyright: (C) 2006 by zhonghonglie
	Function: 声明 IPC 机制的函数原型和全局变量
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#define BUFSZ 256  // 缓冲区大小

// 建立或获取IPC的一组函数的原型说明
int get_ipc_id(char *proc_file, key_t key);
char *set_shm(key_t shm_key, int shm_num, int shm_flag);
int set_msq(key_t msq_key, int msq_flag);
int set_sem(key_t sem_key, int sem_val, int sem_flag);
int down(int sem_id);  // 信号量P操作
int up(int sem_id);    // 信号量V操作

/* 信号灯控制用的共同体 */
typedef union semuns {
	int val;  // 信号量值
} Sem_uns;

/* 消息结构体 */
typedef struct msgbuf {
	long mtype;       // 消息类型，必须大于0
	char mtext[1];    // 消息数据（长度可变）
} Msg_buf;

// 全局变量声明（需在ipc.c中定义）
extern key_t buff_key;     // 缓冲区共享内存键值
extern int buff_num;       // 缓冲区大小（字节数）
extern char *buff_ptr;     // 缓冲区指针

// 生产者放产品位置的共享指针
extern key_t pput_key;     // 生产者指针键值
extern int pput_num;       // 指针数量
extern int *pput_ptr;      // 生产者指针

// 消费者取产品位置的共享指针
extern key_t cget_key;     // 消费者指针键值
extern int cget_num;       // 指针数量
extern int *cget_ptr;      // 消费者指针

// 信号量相关全局变量
extern key_t prod_key;     // 生产者同步信号量键值
extern key_t pmtx_key;     // 生产者互斥信号量键值
extern int prod_sem;       // 生产者同步信号量ID
extern int pmtx_sem;       // 生产者互斥信号量ID

extern key_t cons_key;     // 消费者同步信号量键值
extern key_t cmtx_key;     // 消费者互斥信号量键值
extern int cons_sem;       // 消费者同步信号量ID
extern int cmtx_sem;       // 消费者互斥信号量ID

extern int sem_val;        // 信号量初始值
extern int sem_flg;        // 信号量操作标志
extern int shm_flg;        // 共享内存操作标志