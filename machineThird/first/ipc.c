/*
	Filename: ipc.c
	Copyright: (C) 2006 by zhonghonglie
	Function: 实现 IPC 机制的核心函数
*/
#include "ipc.h"

// 全局变量声明（需在ipc.c中定义）
key_t buff_key;     // 缓冲区共享内存键值
int buff_num;       // 缓冲区大小（字节数）
char *buff_ptr;     // 缓冲区指针

// 生产者放产品位置的共享指针
key_t pput_key;     // 生产者指针键值
int pput_num;       // 指针数量
int *pput_ptr;      // 生产者指针

// 消费者取产品位置的共享指针
key_t cget_key;     // 消费者指针键值
int cget_num;       // 指针数量
int *cget_ptr;      // 消费者指针

// 信号量相关全局变量
key_t prod_key;     // 生产者同步信号量键值
key_t pmtx_key;     // 生产者互斥信号量键值
int prod_sem;       // 生产者同步信号量ID
int pmtx_sem;       // 生产者互斥信号量ID

key_t cons_key;     // 消费者同步信号量键值
key_t cmtx_key;     // 消费者互斥信号量键值
int cons_sem;       // 消费者同步信号量ID
int cmtx_sem;       // 消费者互斥信号量ID

int sem_val;        // 信号量初始值
int sem_flg;        // 信号量操作标志
int shm_flg;        // 共享内存操作标志

/*
	get_ipc_id() - 从/proc/sysvipc/文件系统中获取IPC对象的ID
	@proc_file: 对应的IPC文件路径（msg/sem/shm）
	@key: 要获取的IPC对象键值
*/
int get_ipc_id(char *proc_file, key_t key) {
	FILE *pf;
	int i, j;
	char line[BUFSZ], colum[BUFSZ];

	if ((pf = fopen(proc_file, "r")) == NULL) {
		perror("Proc file not open");
		exit(EXIT_FAILURE);
	}
	fgets(line, BUFSZ, pf); // 跳过首行标题

	while (!feof(pf)) {
		i = j = 0;
		fgets(line, BUFSZ, pf); // 读取数据行

		// 解析键值字段
		while (line[i] == ' ') i++;
		while (line[i] != ' ') colum[j++] = line[i++];
		colum[j] = '\0';

		if (atoi(colum) != key) continue; // 键值匹配检测

		// 提取IPC ID
		j = 0;
		while (line[i] == ' ') i++;
		while (line[i] != ' ') colum[j++] = line[i++];
		colum[j] = '\0';
		fclose(pf);
		return atoi(colum);
	}
	fclose(pf);
	return -1; // 未找到对应IPC对象
}

/*
	down() - 信号量P操作（阻塞）
	@sem_id: 信号量ID
*/
int down(int sem_id) {
	struct sembuf buf;
	buf.sem_op = -1;       // 减1操作
	buf.sem_num = 0;       // 信号量数组下标
	buf.sem_flg = SEM_UNDO; // 自动恢复机制

	if ((semop(sem_id, &buf, 1)) < 0) {
		perror("down error");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}

/*
	up() - 信号量V操作（释放）
	@sem_id: 信号量ID
*/
int up(int sem_id) {
	struct sembuf buf;
	buf.sem_op = 1;       // 加1操作
	buf.sem_num = 0;      // 信号量数组下标
	buf.sem_flg = SEM_UNDO; // 自动恢复机制

	if ((semop(sem_id, &buf, 1)) < 0) {
		perror("up error");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}

/*
	set_sem() - 创建/获取信号量并初始化
	@sem_key: 信号量键值
	@sem_val: 初始值
	@sem_flg: 权限标志
*/
int set_sem(key_t sem_key, int sem_val, int sem_flg) {
	int sem_id;
	Sem_uns sem_arg;

	// 检查信号量是否存在
	if ((sem_id = get_ipc_id("/proc/sysvipc/sem", sem_key)) < 0) {
		// 创建新信号量（1个元素）
		if ((sem_id = semget(sem_key, 1, sem_flg)) < 0) {
			perror("semaphore create error");
			exit(EXIT_FAILURE);
		}
		// 初始化信号量值
		sem_arg.val = sem_val;
		if (semctl(sem_id, 0, SETVAL, sem_arg) < 0) {
			perror("semaphore set error");
			exit(EXIT_FAILURE);
		}
	}
	return sem_id;
}

/*
	set_shm() - 创建/获取共享内存并附加
	@shm_key: 共享内存键值
	@shm_num: 内存大小（字节）
	@shm_flg: 权限标志
*/
char *set_shm(key_t shm_key, int shm_num, int shm_flg) {
	int i, shm_id;
	char *shm_buf;

	// 检查共享内存是否存在
	if ((shm_id = get_ipc_id("/proc/sysvipc/shm", shm_key)) < 0) {
		// 创建新共享内存
		if ((shm_id = shmget(shm_key, shm_num, shm_flg)) < 0) {
			perror("shareMemory set error");
			exit(EXIT_FAILURE);
		}
		// 附加到进程地址空间
		if ((shm_buf = (char *)shmat(shm_id, 0, 0)) < (char *)0) {
			perror("get shareMemory error");
			exit(EXIT_FAILURE);
		}
		for (i = 0; i < shm_num; i++) shm_buf[i] = 0; // 初始化为0
	} else {
		// 附加已有内存
		if ((shm_buf = (char *)shmat(shm_id, 0, 0)) < (char *)0) {
			perror("get shareMemory error");
			exit(EXIT_FAILURE);
		}
	}
	return shm_buf;
}

/*
	set_msq() - 创建/获取消息队列
	@msq_key: 队列键值
	@msq_flg: 权限标志
*/
int set_msq(key_t msq_key, int msq_flg) {
	int msq_id;

	// 检查消息队列是否存在
	if ((msq_id = get_ipc_id("/proc/sysvipc/msg", msq_key)) < 0) {
		// 创建新队列
		if ((msq_id = msgget(msq_key, msq_flg)) < 0) {
			perror("messageQueue set error");
			exit(EXIT_FAILURE);
		}
	}
	return msq_id;
}