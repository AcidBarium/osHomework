import subprocess
import random

# 构造要输入给程序的内容
inputs = []

total_num = 100

inputs.append(str(total_num))

# 生成10个随机数（1-10）作为输入
random_numbers = [str(random.randint(1, 20)) for _ in range(total_num)]
inputs.extend(random_numbers)

sizeOfMemory = 10

inputs.append(str(sizeOfMemory))

# 拼接成程序需要的完整输入字符串，每行一个值，最后加换行
input_str = '\n'.join(inputs) + '\n'

# 运行vmrp.exe程序
process = subprocess.Popen(
    ['./vmrp.exe'],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True  # Python 3.6+，把二进制流转成字符串
)

# 把输入发过去，捕获输出
ans_ori, err = process.communicate(input=input_str)

# 输出原始结果，方便你后续操作
print("程序原始输出：")
print(ans_ori)