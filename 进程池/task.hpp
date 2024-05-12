#pragma once
#include <iostream>
#include <ctime>
#include <cstdlib> //c++����c���Ե�stdlib.hͷ�ļ�
#include <sys/types.h>
#include <unistd.h>

#define TaskNum 3 // ����Ҫ������������͸���

typedef void (*task_t)(); // task_t ����ָ�����ͣ�task_t��һ�����Ͳ���һ��ָ�룩

task_t tasks[TaskNum]; // ����һ��task_t����ָ�����͵�����tasks,�����д�ŵĶ��Ǻ���ָ��

// ��ӡ����
void Print()
{
    std::cout << "I am a print task" << std::endl;
}

// ��������
void DownLoad()
{
    std::cout << "I am a DownLoad task" << std::endl;
}

// ˢ������
void Flush()
{
    std::cout << "I am a Flush task" << std::endl;
}

// �������񣨽�������뺯��ָ�������У�
void LoadTask()
{
    srand(time(nullptr) ^ getpid() ^ 17777); // ����ʱ��� �� ��ǰ���̵�pid���Ľ��ʹ�á����ӡ����ӵ��������ȻҲ�������������������
    tasks[0] = Print;                        // ��һ������ָ��ָ���ӡ����
    tasks[1] = DownLoad;                     // �ڶ�������ָ��ָ����������
    tasks[2] = Flush;                        // ����������ָ��ָ��ˢ������
}

// ִ������
void ExcuteTask(int number)
{
    if (number < 0 || number > 2)
        return;
    tasks[number](); // ���ݴ����������ȷ��Ҫ���õĺ���
}

// ѡ��������
int SelectTask()
{
    return rand() % TaskNum; // ���������������
}

// // �汾һ��
// //  �ӽ��̴����ɷ��������ӽ��̻������rfd�ӹܵ����õ������룩
// void work(int rfd)
// {
//     // �ӽ���ѭ���ȴ�
//     int i = 1;
//     while (1)
//     {
//         int command = 0;
//         int n = read(rfd, &command, sizeof(command)); // OS������rfd�����ӽ��̻�ȡ���������Ĺܵ��е�����
//         if (n == sizeof(int))
//         {
//             std::cout << "pid = " << getpid() << " ���ӽ�������ִ������" << std::endl;
//             ExcuteTask(command); // ����������ִ������
//             std::cout << std::endl;
//         }
//         else if (n == 0) // ���˶�ȡ��������ʱ�����ӽ��̵�work
//         {
//             std::cout << "pid = " << getpid() << " ���ӽ��̶�ȡ����������" << std::endl;
//             break;
//         }
//     }
// }

// �汾����
// �ӽ��̵�����
void work()
{
    // �ӽ���ѭ����������
    int i = 1;
    while (1)
    {
        int command = 0;
        int n = read(0, &command, sizeof(command)); // OS������rfd�����ӽ��̻�ȡ���������Ĺܵ��е�����
        std::cout << std::endl;
        if (n == sizeof(int))
        {
            std::cout << "pid = " << getpid() << " ���ӽ�������ִ������" << std::endl;
            ExcuteTask(command); // ����������ִ������
            std::cout << std::endl;
            sleep(1);
        }
        else if (n == 0) // ���˶�ȡ��������ʱ�����ӽ��̵�work
        {
            std::cout << "pid = " << getpid() << " ���ӽ��̶�ȡ����������" << std::endl;
            break;
        }
    }
}