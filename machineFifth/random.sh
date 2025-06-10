#!/bin/bash

# 检查dask文件是否存在并可执行
if [ ! -x "./dask" ]; then
    echo "错误: ./dask 文件不存在或不可执行"
    exit 1
fi

# 生成0-200的随机数
random1=$((RANDOM % 201))
echo "生成的第一个随机数: $random1"

# 生成0或1
binary=$((RANDOM % 2))
echo "生成的0或1: $binary"

# 生成n (1-20之间的随机数，可以根据需要调整范围)
n=$((RANDOM % 20 + 1))
echo "生成的n值: $n"

# 生成n个0-200的随机数
randoms=()
for ((i=0; i<n; i++)); do
    randoms+=($((RANDOM % 201)))
done
echo "生成的$n个随机数: ${randoms[@]}"

# 运行dask程序并输入生成的数据
{
    echo "$random1"
    echo "$binary"
    echo "$n"
    for num in "${randoms[@]}"; do
        echo "$num"
    done
} | ./dask

echo "程序执行完成"