/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-10 21:01:58
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-11 22:33:53
 * @FilePath: /2024.5.10/Task.hpp
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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
task_t Print()
{
    std::cout << "I am a print task" << std::endl;
    return 0;
}

// ��������
task_t DownLoad()
{
    std::cout << "I am a DownLoad task" << std::endl;
    return 0;
}

// ˢ������
task_t Flush()
{
    std::cout << "I am a Flush task" << std::endl;
    return 0;
}

// �������񣨽�������뺯��ָ�������У�
void LoadTask()
{
    srand(time(nullptr) ^ getpid() ^ 17777); // ����ʱ��� �� ��ǰ���̵�pid���Ľ��ʹ�á����ӡ����ӵ��������ȻҲ�������������������
    tasks[0] = Print();                      // ��һ������ָ��ָ���ӡ����
    tasks[1] = DownLoad();                   // �ڶ�������ָ��ָ����������
    tasks[2] = Flush();                      // ����������ָ��ָ��ˢ������
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