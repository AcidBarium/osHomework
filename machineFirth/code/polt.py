import subprocess
import random
import pandas as pd
import re
import pandas as pd
import matplotlib.pyplot as plt

# 定义算法名称
methods = ['FIFO', 'LRU', 'Clock', 'EClock', 'LFU', 'MFU']

# 结果字典，key是方法，value是每个sizeOfMemory下的page fault数量列表
results = {method: [] for method in methods}

# 设定总页数
total_num = 1000

# 横坐标：sizeOfMemory 1~10
size_range = range(1, 50)

# 遍历不同sizeOfMemory
for sizeOfMemory in size_range:
    # 构造输入内容
    inputs = [str(total_num)]
    random_numbers = [str(random.randint(1, 50)) for _ in range(total_num)]
    inputs.extend(random_numbers)
    inputs.append(str(sizeOfMemory))
    input_str = '\n'.join(inputs) + '\n'

    # 运行程序
    process = subprocess.Popen(
        ['./vmrp.exe'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    ans_ori, err = process.communicate(input=input_str)

    # 从输出中用正则提取每个方法的Number of page faults
    for method in methods:
        pattern = rf"{method}.*?Number of page faults = (\d+)"
        match = re.search(pattern, ans_ori, re.DOTALL)
        if match:
            faults = int(match.group(1))
        else:
            faults = None  # 如果没匹配到，记录None
        results[method].append(faults)

# 把结果转成DataFrame，index是sizeOfMemory
df = pd.DataFrame(results, index=size_range)
df.index.name = 'Size of Memory'

# 写入Excel
df.to_excel('result.xlsx')

print("✅ 数据已保存到 result.xlsx！")


# 读取Excel文件
df = pd.read_excel('result.xlsx', index_col=0)

# 设置绘图风格
plt.style.use('seaborn-v0_8-darkgrid')

# 创建图形和坐标轴
plt.figure(figsize=(10, 6))

# 遍历每个方法，画一条折线
for column in df.columns:
    plt.plot(df.index, df[column], marker='o', label=column)

# 设置标题和坐标轴标签
plt.title('Page Faults vs. Memory Size')
plt.xlabel('Size of Memory (Page Frames)')
plt.ylabel('Number of Page Faults')

# 显示图例
plt.legend()

# 显示图形
plt.tight_layout()
plt.show()