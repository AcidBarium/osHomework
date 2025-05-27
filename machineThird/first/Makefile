# Makefile for Producer-Consumer实验
# 来源：实验手册4.3节示例 [[1]]

# 定义头文件和编译选项
hdrs = ipc.h
opts = -g -c  # 使用调试模式编译（-g）并生成目标文件（-c）

# 消费者相关源文件和目标文件
c_src = consumer.c ipc.c
c_obj = consumer.o ipc.o

# 生产者相关源文件和目标文件
p_src = producer.c ipc.c
p_obj = producer.o ipc.o

# 默认目标：生成producer和consumer可执行文件
all: producer consumer

# 生成消费者可执行文件
consumer: $(c_obj)
	gcc $(c_obj) -o consumer

# 生成消费者目标文件
consumer.o: $(c_src) $(hdrs)
	gcc $(opts) $(c_src)

producer: $(p_obj)
	gcc $(p_obj) -o producer

# 生成生产者目标文件
producer.o: $(p_src) $(hdrs)
	gcc $(opts) $(p_src)

# 清理生成的文件
clean:
	rm -f consumer producer *.o