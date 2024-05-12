#pragma once
#include <iostream>
#include <ctime>
#include <cstdlib> //c++风格的c语言的stdlib.h头文件
#include <sys/types.h>
#include <unistd.h>

#define TaskNum 3 // 定义要处理的任务类型个数

typedef void (*task_t)(); // task_t 函数指针类型（task_t是一个类型不是一个指针）

task_t tasks[TaskNum]; // 创建一个task_t函数指针类型的数组tasks,数组中存放的都是函数指针

// 打印任务
void Print()
{
    std::cout << "I am a print task" << std::endl;
}

// 下载任务
void DownLoad()
{
    std::cout << "I am a DownLoad task" << std::endl;
}

// 刷新任务
void Flush()
{
    std::cout << "I am a Flush task" << std::endl;
}

// 加载任务（将任务放入函数指针数组中）
void LoadTask()
{
    srand(time(nullptr) ^ getpid() ^ 17777); // 依据时间戳 与 当前进程的pid亦或的结果使得“种子”更加的随机，当然也可以再亦或上其它内容
    tasks[0] = Print;                        // 第一个函数指针指向打印任务
    tasks[1] = DownLoad;                     // 第二个函数指针指向下载任务
    tasks[2] = Flush;                        // 第三个函数指针指向刷新任务
}

// 执行任务
void ExcuteTask(int number)
{
    if (number < 0 || number > 2)
        return;
    tasks[number](); // 根据传入的任务码确定要调用的函数
}

// 选择任务码
int SelectTask()
{
    return rand() % TaskNum; // 返回随机的任务码
}

// // 版本一：
// //  子进程处理派发的任务（子进程会从依据rfd从管道中拿到任务码）
// void work(int rfd)
// {
//     // 子进程循环等待
//     int i = 1;
//     while (1)
//     {
//         int command = 0;
//         int n = read(rfd, &command, sizeof(command)); // OS会依据rfd帮助子进程获取与它关联的管道中的内容
//         if (n == sizeof(int))
//         {
//             std::cout << "pid = " << getpid() << " 的子进程正在执行任务" << std::endl;
//             ExcuteTask(command); // 依据任务码执行任务
//             std::cout << std::endl;
//         }
//         else if (n == 0) // 读端读取不到内容时结束子进程的work
//         {
//             std::cout << "pid = " << getpid() << " 的子进程读取不到内容了" << std::endl;
//             break;
//         }
//     }
// }

// 版本二：
// 子进程的任务
void work()
{
    // 子进程循环处理任务
    int i = 1;
    while (1)
    {
        int command = 0;
        int n = read(0, &command, sizeof(command)); // OS会依据rfd帮助子进程获取与它关联的管道中的内容
        std::cout << std::endl;
        if (n == sizeof(int))
        {
            std::cout << "pid = " << getpid() << " 的子进程正在执行任务" << std::endl;
            ExcuteTask(command); // 依据任务码执行任务
            std::cout << std::endl;
            sleep(1);
        }
        else if (n == 0) // 读端读取不到内容时结束子进程的work
        {
            std::cout << "pid = " << getpid() << " 的子进程读取不到内容了" << std::endl;
            break;
        }
    }
}