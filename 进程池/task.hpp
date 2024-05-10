#pragma once

#include <iostream>

#define TaskNum 3//����Ҫ������������͸���

typedef void (*task_t)();//task_t ����ָ�����ͣ�task_t��һ�����Ͳ���һ��ָ�룩

task_t tasks[TaskNum];//����һ��task_t����ָ�����͵�����tasks,�����д�ŵĶ��Ǻ���ָ��

class TaskList//�����б��࣬���ڴ�ſ���ִ�е�����
{
public:
    // ��ӡ����
    task_t Print()
    {
        std::cout << "I am a print task" << std::endl;
    }

    // ��������
    task_t DownLoad()
    {
        std::cout << "I am a DownLoad task" << std::endl;
    }

    // ˢ������
    task_t Flush()
    {
        std::cout << "I am a Flush task" << std::endl;
    }
};


//�������񣨽�������뺯��ָ�������У�
void LoadTask()
{
    TaskList TL;
    tasks[0] = TL.Print();//��һ������ָ��ָ���ӡ����
    tasks[1] = TL.DownLoad();//�ڶ�������ָ��ָ����������
    tasks[2] = TL.Flush();//����������ָ��ָ��ˢ������
}

//ִ������ 
void ExcuteTask(int number)
{
    if (number < 0 || number > 2) return;
    tasks[number]();//
}