#pragma once

#include <iostream>

#define TaskNum 3//定义要处理的任务类型个数

typedef void (*task_t)();//task_t 函数指针类型（task_t是一个类型不是一个指针）

task_t tasks[TaskNum];//创建一个task_t函数指针类型的数组tasks,数组中存放的都是函数指针

class TaskList//任务列表类，用于存放可以执行的任务
{
public:
    // 打印任务
    task_t Print()
    {
        std::cout << "I am a print task" << std::endl;
    }

    // 下载任务
    task_t DownLoad()
    {
        std::cout << "I am a DownLoad task" << std::endl;
    }

    // 刷新任务
    task_t Flush()
    {
        std::cout << "I am a Flush task" << std::endl;
    }
};


//加载任务（将任务放入函数指针数组中）
void LoadTask()
{
    TaskList TL;
    tasks[0] = TL.Print();//第一个函数指针指向打印任务
    tasks[1] = TL.DownLoad();//第二个函数指针指向下载任务
    tasks[2] = TL.Flush();//第三个函数指针指向刷新任务
}

//执行任务 
void ExcuteTask(int number)
{
    if (number < 0 || number > 2) return;
    tasks[number]();//
}