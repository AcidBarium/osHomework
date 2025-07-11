#include "vmrp.h"
Replace::Replace()
{
    int i; // 设定总得访问页数,并分配相应的引用页号和淘汰页号记录数组空间
    cout << "Please input page numbers :";
    cin >> PageNumber;
    ReferencePage = new int[sizeof(int) * PageNumber];
    EliminatePage = new int[sizeof(int) * PageNumber];
    // 输入引用页号序列(页面走向), 初始化引用页数组cout
    cout << "Please input reference page string :";
    for (i = 0; i < PageNumber; i++)
        cin >> ReferencePage[i]; // 引用页暂存引用数组
    // 设定内存实页数(帧数),并分配相应的实页号记录数组空间(页号栈)
    cout << "Please input page frames :";
    cin >> FrameNumber;
    PageFrames = new int[sizeof(int) * FrameNumber];
}
Replace::~Replace() {}
void Replace::InitSpace(char *MethodName)
{
    int i;
    cout << endl
         << MethodName << endl;
    FaultNumber = 0;
    // 引用还未开始,-1 表示无引用页
    for (i = 0; i < PageNumber; i++)
        EliminatePage[i] = -1;
    for (i = 0; i < FrameNumber; i++)
        PageFrames[i] = -1;
}
void Replace::Report(void)
{
    // 报告淘汰页顺序
    cout << endl
         << "Eliminate page:";
    for (int i = 0; EliminatePage[i] != -1; i++)
        cout << EliminatePage[i] << " ";
    // 报告缺页数和缺页率
    cout << endl
         << "Number of page faults = " << FaultNumber << endl;
    cout << setw(6) << setprecision(3);
    cout << "Rate of page faults = "
         << 100 * (float)FaultNumber / (float)PageNumber << "%" << endl;
}
void Replace::Lru(void) // 最近最久未使用的页面被淘汰。
{
    int i, j, k, l, next;
    InitSpace("LRU");
    for (k = 0, l = 0; k < PageNumber; k++)
    {
        next = ReferencePage[k];
        // 检测引用页当前是否已在实存
        for (i = 0; i < FrameNumber; i++)
        {
            if (next == PageFrames[i])
            {
                // 引用页已在实存将其调整到页记录栈顶
                next = PageFrames[i];
                for (j = i; j > 0; j--)
                    PageFrames[j] = PageFrames[j - 1];
                PageFrames[0] = next;
                break;
            }
        }
        if (PageFrames[0] == next)
        {
            // 如果引用页已放栈顶，则为不缺页，报告当前内存页号
            for (j = 0; j < FrameNumber; j++)
                if (PageFrames[j] >= 0)
                    cout << PageFrames[j] << " ";
            cout << endl;
            continue;
        }
        else
            // 如果引用页还未放栈顶，则为缺页，缺页数加1
            FaultNumber++;
        // 栈底页号记入淘汰页数组中
        EliminatePage[l] = PageFrames[FrameNumber - 1];
        // 向下压栈
        for (j = FrameNumber - 1; j > 0; j--)
            PageFrames[j] = PageFrames[j - 1];
        PageFrames[0] = next; // 引|用页放栈顶
        // 报告当前实存中页号
        for (j = 0; j < FrameNumber; j++)
            if (PageFrames[j] >= 0)
                cout << PageFrames[j] << " ";
        // 报告当前淘汰的页号
        if (EliminatePage[l] >= 0)
            cout << "->" << EliminatePage[l++] << endl;
        else
            cout << endl;
    }
    // 分析统计选择的算法对于当前引用的页面走向的性能
    Report();
}
void Replace::Fifo(void) // 最先进入内存的页面，最先被淘汰。
{
    int i, j, k, l, next;
    InitSpace("FIFO");
    // 循环装入引用页
    for (k = 0, j = l = 0; k < PageNumber; k++)
    {
        next = ReferencePage[k];
        // 如果引用页已在实存中，报告实存页号
        for (i = 0; i < FrameNumber; i++)
            if (next == PageFrames[i])
                break;
        if (i < FrameNumber)
        {
            for (i = 0; i < FrameNumber; i++)
                cout << PageFrames[i] << " ";
            cout << endl;
            continue;
        }
        // 引用页不在实存中，缺页数加1
        FaultNumber++;
        EliminatePage[l] = PageFrames[j];
        // 最先入页号记入淘汰页数组
        PageFrames[j] = next;      // 引用页号放最先入页号处
        j = (j + 1) % FrameNumber; // 最先入页号循环下移
        // 报告当前实存页号和淘汰页号
        for (i = 0; i < FrameNumber; i++)
            if (PageFrames[i] >= 0)
                cout << PageFrames[i] << " ";
        if (EliminatePage[l] >= 0)
            cout << "->" << EliminatePage[l++] << endl;
        else
            cout << endl;
    }
    // 分析统计选择的算法对于当前引用的页面走向的性能
    Report();
}

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
            if (PageFrames[cnt] != -1)
            {
                EliminatePage[eliminateIndex++] = PageFrames[cnt];
            }
            
            FaultNumber++;
            PageFrames[cnt] = next;
            frequency[cnt] = 0; // 新页面初始使用频率为0

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

int main(int argc, char *argv[])
{
    Replace *vmpr = new Replace();
    vmpr->Fifo();
    vmpr->Lru();
    vmpr->Clock();
    vmpr->Eclock();
    vmpr->Lfu();
    vmpr->Mfu();
    return 0;
}