# 操作系统第四次上机报告

姓名：刘晨旭 学号：20232241110 班级：软件2306

仓库地址：https://github.com/AcidBarium/osHomework

## 实验目的

本实验旨在加深学生对虚拟存储器及存储管理原理的理解，通过模拟页面置换算法（如LRU和FIFO），观察其置换过程，掌握编程技巧，并分析实验数据以比较不同算法的性能。实验要求学生测试不同引用串和内存帧数的组合，绘制缺页数曲线图，并综合分析算法适应性，从而写出实验报告。

## 实验过程

- 创建文件：新建vmrp.h和vmrp.cc文件，用于声明和定义虚拟内存页置换类及其相关算法函数。
- 编写程序代码：
> 在vmrp.h文件中，声明虚拟内存页置换类Replace，包括页面置换算法相关的函数，如InitSpace、Report、Fifo、Lru等。
在vmrp.cc文件中，实现类的构造函数、析构函数、InitSpace函数用于初始化页号记录，Report函数用于报告算法执行情况，以及Fifo和Lru函数分别实现先进先出和最近最久未用置换算法。

- 创建Makefile文件：用于编译和链接程序代码，生成可执行文件vmpr。
- 编译程序：通过执行make命令，按照Makefile文件中的规则编译vmrp.cc文件，生成目标文件vmrp.o，再将其链接生成可执行文件vmpr。
- 测试程序:运行可执行文件vmpr，输入相应的页面引用串和内存帧数。

## 实验代码以及详细解释

### Clock

```cpp
void Replace::Clock(void) // 循环扫描，找到 used=false 的页面淘汰，否则把 used 清 0。
{
    InitSpace("Clock");
    int pointer = 0;                    // 指向下一个要替换的页面
    bool *used = new bool[FrameNumber]; // 用于记录页面是否被访问过
    int eliminateIndex = 0;             // 用于记录淘汰页的索引

    for (int i = 0; i < FrameNumber; i++)
    {
        used[i] = false;
    }

    for (int k = 0; k < PageNumber; k++)
    {
        int next = ReferencePage[k];
        bool found = false;

        // 检查页面是否在帧中
        for (int i = 0; i < FrameNumber; i++)
        {
            if (PageFrames[i] == next)
            {
                found = true;
                used[i] = true; // 将页面标记为已访问
                break;
            }
        }

        if (!found)
        {
            FaultNumber++;
            // 找到一个没有被访问过的页面进行替换
            while (used[pointer])
            {
                used[pointer] = false;
                pointer = (pointer + 1) % FrameNumber;
            }

            // 记录被淘汰的页面
            if (PageFrames[pointer] != -1)
            {
                EliminatePage[eliminateIndex++] = PageFrames[pointer];
            }

            // 进行页面替换
            PageFrames[pointer] = next;
            used[pointer] = true;
            pointer = (pointer + 1) % FrameNumber;
        }

        // 报告当前实存中页号
        for (int j = 0; j < FrameNumber; j++)
        {
            if (PageFrames[j] >= 0)
                cout << PageFrames[j] << " ";
        }
        if (!found && eliminateIndex > 0 &&
            EliminatePage[eliminateIndex - 1] > 0)
            cout << "->" << EliminatePage[eliminateIndex - 1] << endl;
        else
            cout << endl;
    }

    delete[] used;
    Report();
}
```


一种近似 LRU 的算法。每个页面有个 `used` 标志位：

* 页面命中：`used=1`
* 缺页时：

  * 顺时针找 `used=0` 的页面淘汰
  * 如果 `used=1`，先清 0，继续找
  * 找到 `used=0` 就替换，`used=1`


### Eclock

```cpp
void Replace::Eclock(void) // 在 Clock 基础上，考虑 used 和 modified，优先淘汰 used=0, modified=0 的。
{
    InitSpace("EClock");
    int pointer = 0;                        // 指向下一个要替换的页面
    bool *used = new bool[FrameNumber];     // 用于记录页面是否被访问过
    bool *modified = new bool[FrameNumber]; // 用于记录页面是否被修改过
    int eliminateIndex = 0;                 // 用于记录淘汰页的索引

    for (int i = 0; i < FrameNumber; i++)
    {
        used[i] = false;
        modified[i] = false;
    }

    for (int k = 0; k < PageNumber; k++)
    {
        int next = ReferencePage[k];
        bool found = false;

        // 检查页面是否在帧中
        for (int i = 0; i < FrameNumber; i++)
        {
            if (PageFrames[i] == next)
            {
                found = true;
                used[i] = true;     // 将页面标记为已访问
                modified[i] = true; // 假设页面被访问时也被修改
                break;
            }
        }

        if (!found)
        {
            FaultNumber++;
            // 找到一个没有被访问过的页面进行替换
            //  0 0 立即替换

            int zeroOne = -1;

            int pointerTemp = pointer;
            pointerTemp++;
            pointerTemp %= FrameNumber;

            bool isHaveZeroZero = false;

            while (pointerTemp != pointer) // 先找一圈有没有00
            {
                if (used[pointerTemp] == false && modified[pointerTemp] == false)
                {
                    isHaveZeroZero = true;
                    break;
                }
                pointerTemp = (pointerTemp + 1) % FrameNumber;
            }

            if (used[pointer] == false && modified[pointer] == false)
            {
                isHaveZeroZero = true;
            }

            if (isHaveZeroZero)
            {
                while (1)
                {
                    if (used[pointer] == false && modified[pointer] == false)
                    {
                        // 00情况：直接置换
                        break;
                    }
                    else if (used[pointer] == false && modified[pointer] == true)
                    {
                    }
                    else if (used[pointer] == true && modified[pointer] == false)
                    {
                        // 10情况：R清0
                        used[pointer] = 0;
                    }
                    else if (used[pointer] == true && modified[pointer] == true)
                    {
                        // 11情况：R清0
                        used[pointer] = 0;
                    }
                    // 循环下一个
                    pointer = (pointer + 1) % FrameNumber;
                }
            }
            else
            {
                while (1)
                {
                    if (used[pointer] == false && modified[pointer] == false)
                    {
                        // 00情况：直接置换
                        break;
                    }
                    else if (used[pointer] == false && modified[pointer] == true)
                    {
                        break;
                    }
                    else if (used[pointer] == true && modified[pointer] == false)
                    {
                        // 10情况：R清0
                        used[pointer] = 0;
                    }
                    else if (used[pointer] == true && modified[pointer] == true)
                    {
                        // 11情况：R清0
                        used[pointer] = 0;
                    }

                    // 循环下一个
                    pointer = (pointer + 1) % FrameNumber;
                }
            }

            // 记录被淘汰的页面
            if (PageFrames[pointer] != -1)
            {
                EliminatePage[eliminateIndex++] = PageFrames[pointer];
            }

            // 进行页面替换
            PageFrames[pointer] = next;
            used[pointer] = true;
            modified[pointer] = false; // 新页面假设未被修改
            pointer = (pointer + 1) % FrameNumber;
        }

        // 报告当前实存中页号
        for (int j = 0; j < FrameNumber; j++)
        {
            if (PageFrames[j] >= 0)
                cout << PageFrames[j] << " ";
        }
        if (!found && eliminateIndex > 0 &&
            EliminatePage[eliminateIndex - 1] > 0)
            cout << "->" << EliminatePage[eliminateIndex - 1] << endl;
        else
            cout << endl;
    }

    delete[] used;
    delete[] modified;
    Report();
}
```


在普通 **Clock** 算法基础上，额外引入 `modified` 位，淘汰优先级：

**优先顺序**：

1. `used=0, modified=0` 立即淘汰
2. 没有 00 时，`used=0, modified=1` 淘汰（需写回），此过程中将经过的`used`改为0 。直到发现`00` 或者 `01`

在寻找是否存在00的时候是无修改查询，在进行选择的过程中是有修改的。

**页面命中**：

* `used=1`，`modified=1`（假设访问就会修改）

**算法流程**：

* 缺页时，先一圈找 00
* 没有 00，再找 01
* 实在没有，清除所有 `used=1`，再循环找
* 找到符合条件的页进行置换

### Lfu
```cpp
void Replace::Lfu(void) // 淘汰使用频率最少的页面。
{
    InitSpace("LFU");
    int *frequency = new int[FrameNumber]; // 记录每个页面的使用频率
    int eliminateIndex = 0;                // 用于记录淘汰页的索引

    for (int i = 0; i < FrameNumber; i++)
    {
        frequency[i] = 0;
    }

    for (int k = 0; k < PageNumber; k++)
    {
        int next = ReferencePage[k];
        bool found = false;

        // 检查页面是否在帧中
        for (int i = 0; i < FrameNumber; i++)
        {
            if (PageFrames[i] == next)
            {
                found = true;
                frequency[i]++; // 增加页面的使用频率
                break;
            }
        }

        if (!found)
        {
            FaultNumber++;
            // 找到使用频率最低的页面进行替换
            int minFreqIndex = 0;
            for (int i = 1; i < FrameNumber; i++)
            {
                if (frequency[i] < frequency[minFreqIndex])
                {
                    minFreqIndex = i;
                }
            }

            // 记录被淘汰的页面
            if (PageFrames[minFreqIndex] != -1)
            {
                EliminatePage[eliminateIndex++] = PageFrames[minFreqIndex];
            }

            // 进行页面替换
            PageFrames[minFreqIndex] = next;
            frequency[minFreqIndex] = 1; // 新页面初始使用频率为1
        }

        // 报告当前实存中页号
        for (int j = 0; j < FrameNumber; j++)
        {
            if (PageFrames[j] >= 0)
                cout << PageFrames[j] << " ";
        }
        if (!found && eliminateIndex > 0 &&
            EliminatePage[eliminateIndex - 1] > 0)
            cout << "->" << EliminatePage[eliminateIndex - 1] << endl;
        else
            cout << endl;
    }

    delete[] frequency;
    Report();
}
```

**LFU (Least Frequently Used) 页面置换算法**
每次缺页，淘汰**访问频率最低**的页面。页面命中时频率+1，缺页时替换频率最小者，新页频率初始化为 1。


### Mfu

```cpp
void Replace::Mfu(void) // 淘汰使用频率最多的页面。
{
    InitSpace("MFU");
    int *frequency = new int[FrameNumber]; // 记录每个页面的使用频率
    int eliminateIndex = 0;                // 用于记录淘汰页的索引
    int cnt = 0;

    for (int i = 0; i < FrameNumber; i++)
    {
        frequency[i] = 0;
    }

    for (int k = 0; k < PageNumber; k++)
    {
        int next = ReferencePage[k];
        bool found = false;

        // 检查页面是否在帧中
        for (int i = 0; i < FrameNumber; i++)
        {
            if (PageFrames[i] == next)
            {
                found = true;
                frequency[i]++; // 增加页面的使用频率
                break;
            }
        }

        if (!found && cnt < FrameNumber)
        {
            FaultNumber++;
            PageFrames[cnt] = next;
            frequency[cnt] = 0; // 新页面初始使用频率为0

            if (PageFrames[cnt] != -1)
            {
                EliminatePage[eliminateIndex++] = PageFrames[cnt];
            }

            cnt++;
        }
        else if (!found)
        {
            FaultNumber++;
            // 找到使用频率最高的页面进行替换
            int maxFreqIndex = 0;
            for (int i = 1; i < FrameNumber; i++)
            {
                if (frequency[i] >= frequency[maxFreqIndex])
                {
                    maxFreqIndex = i;
                }
            }

            // 记录被淘汰的页面
            if (PageFrames[maxFreqIndex] != -1)
            {
                EliminatePage[eliminateIndex++] = PageFrames[maxFreqIndex];
            }

            // 进行页面替换
            PageFrames[maxFreqIndex] = next;
            frequency[maxFreqIndex] = 0; // 新页面初始使用频率为0
        }

        // 报告当前实存中页号
        for (int j = 0; j < FrameNumber; j++)
        {
            if (PageFrames[j] >= 0)
                cout << PageFrames[j] << " ";
        }
        if (!found && eliminateIndex > 0 &&
            EliminatePage[eliminateIndex - 1] > 0)
            cout << "->" << EliminatePage[eliminateIndex - 1] << endl;
        else
            cout << endl;
    }

    delete[] frequency;
    Report();
}
```

**策略**：缺页时淘汰**访问频率最高**的页面。

**实现细节**：

* 初始化 `frequency[i]=0`，`cnt=0` 表示当前占用帧数。
* 每次访问：

  1. 若页面命中，频率+1。
  2. 若缺页：

     * 若有空帧 (`cnt < FrameNumber`)，直接放入，频率设0，`cnt++`。
     * 否则，找**频率最大**的帧（相等取后者），淘汰，放入新页，频率设0。
* 实时输出当前帧内容，记录淘汰页。


## 实验结果如下所示

```bash
FIFO
1 
1 2 
1 2 3 
4 2 3 ->1
4 1 3 ->2
4 1 2 ->3
5 1 2 ->4
5 1 2 
5 1 2 
5 3 2 ->1
5 3 4 ->2
5 3 4 

Eliminate page:1 2 3 4 1 2 
Number of page faults = 9
Rate of page faults = 75%

LRU
1 
2 1 
3 2 1 
4 3 2 ->1
1 4 3 ->2
2 1 4 ->3
5 2 1 ->4
1 5 2 
2 1 5 
3 2 1 ->5
4 3 2 ->1
5 4 3 ->2

Eliminate page:1 2 3 4 5 1 2 
Number of page faults = 10
Rate of page faults = 83.3%

Clock
1 
1 2 
1 2 3 
4 2 3 ->1
4 1 3 ->2
4 1 2 ->3
5 1 2 ->4
5 1 2 
5 1 2 
5 3 2 ->1
5 3 4 ->2
5 3 4 

Eliminate page:1 2 3 4 1 2 
Number of page faults = 9
Rate of page faults = 75%

EClock
1 
1 2 
1 2 3 
4 2 3 ->1
4 1 3 ->2
4 1 2 ->3
5 1 2 ->4
5 1 2 
5 1 2 
5 3 2 ->1
4 3 2 ->5
4 3 5 ->2

Eliminate page:1 2 3 4 1 5 2 
Number of page faults = 10
Rate of page faults = 83.3%

LFU
1 
1 2 
1 2 3 
4 2 3 ->1
1 2 3 ->4
1 2 3 
5 2 3 ->1
1 2 3 ->5
1 2 3 
1 2 3 
4 2 3 ->1
5 2 3 ->4

Eliminate page:1 4 1 5 1 4 
Number of page faults = 9
Rate of page faults = 75%

MFU
1 
1 2 
1 2 3 
1 2 4 ->3
1 2 4 
1 2 4 
1 5 4 ->2
1 5 4 
2 5 4 ->1
2 5 3 ->4
2 5 4 ->3
2 5 4 

Eliminate page:1 2 3 3 2 1 4 3 
Number of page faults = 8
Rate of page faults = 66.7%

```

## 心得体会

通过本次实验，我更加深入理解了虚拟存储管理中各种页面置换算法的实现原理与适用场景。不同算法在面对相同引用串和帧数时，缺页率差异明显，体现了算法策略对系统性能的影响。尤其是 Clock 和 LFU 算法，通过简单标志位或计数方式，能够较好地权衡性能与实现复杂度。实践过程中也提升了自己 C++ 类与数据结构操作的熟练度，对 Makefile 工具链配置和调试流程有了更清晰的掌握。
