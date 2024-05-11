/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-10 21:01:58
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-11 22:33:53
 * @FilePath: /2024.5.10/Task.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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
task_t Print()
{
    std::cout << "I am a print task" << std::endl;
    return 0;
}

// 下载任务
task_t DownLoad()
{
    std::cout << "I am a DownLoad task" << std::endl;
    return 0;
}

// 刷新任务
task_t Flush()
{
    std::cout << "I am a Flush task" << std::endl;
    return 0;
}

// 加载任务（将任务放入函数指针数组中）
void LoadTask()
{
    srand(time(nullptr) ^ getpid() ^ 17777); // 依据时间戳 与 当前进程的pid亦或的结果使得“种子”更加的随机，当然也可以再亦或上其它内容
    tasks[0] = Print();                      // 第一个函数指针指向打印任务
    tasks[1] = DownLoad();                   // 第二个函数指针指向下载任务
    tasks[2] = Flush();                      // 第三个函数指针指向刷新任务
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