#include "task.hpp"

#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>

 // 管道
class Channel
{
public:
    Channel(int wfd, pid_t id, const std::string& name)
        : _wfd(wfd), _subprocessid(id), _name(name)
    {
    }

    ~Channel()
    {
    }

    // 获取写端的wfd、目标子进程的pid，当前管道名
    int GetWfd() { return _wfd; }
    pid_t GetProcessID() { return _subprocessid; }
    std::string GetName() { return _name; }

    int GetWfd() const { return _wfd; }
    pid_t GetProcessID() const { return _subprocessid; }
    std::string GetName() const { return _name; }

private:
    int _wfd;
    pid_t _subprocessid;
    std::string _name;
};

// 读端操作
void work(int rfd)
{
    // 子进程循环等待
    while (true)
    {
        sleep(1);
    }
}

// 形参命名规范
// const & 修饰的应该是一个输入型参数
//& 修饰的应该是一个输入输出型参数
//* 修饰的应该是一个输出型参数

// 创建管道和进程池
void CreatChannelAndSub(int num, std::vector<Channel>* channels)
{
    for (int i = 0; i < num; i++) // 循环创建子进程和对应的管道
    {
        // 1、创建管道
        int pipefd[2] = { 0 };
        int n = pipe(pipefd); // 接收创建管道函数的返回值,并调用pipe函数由OS向数组中写入文件描述符，数组首元素OS分配的是rfd，第二个元素是OS分配的wfd
        if (n < 0)
            exit(1); // 创建管道失败进程退出

        // 2、创建子进程
        pid_t id = fork();
        if (id == 0)
        {
            // 子进程----读端
            close(pipefd[1]); // 关闭子进程的wfd
            work(pipefd[0]);  // 子进程处理读取管道中的内容
            close(pipefd[0]); // 关闭子进程的rfd
            exit(0);          // 子进程退出
        }
        // 3、父进程构建管道名
        std::string Channel_name = "Channel-" + std::to_string(i); // 每次循环i+1，管道名即为Channel-i

        // 父进程----写端
        close(pipefd[0]); // 关闭父进程的rfd

        // 父进程需要告诉管道：写端的管道的wfd、子进程的pid,管道的名称
        channels->push_back(Channel(pipefd[1], id, Channel_name));
    }
}

// 检测进程池和管道是否创建成功
void TestForProcessPoolAndSub(const std::vector<Channel>& channels)
{

    for (auto& Channel : channels)
    {
        std::cout << "=======================================================" << std::endl;
        std::cout << Channel.GetName() << "-" << Channel.GetProcessID() << "-" << Channel.GetWfd() << std::endl;
    }
}

// 创建进有五个进程的进程池，在命令行中的命令行字符串是./processpool 5,一共有命令行字符串的数量应该为2时才能进行创建进程池
int main(int agrc, char* argv[])
{

    if (agrc != 2) // 命令行参数不为2那么就报错并返回
    {
        std::cerr << "Usage: " << argv[0] << " processnum" << std::endl;
        return 1;
    }

    int num = std::stoi(argv[1]); // 将argv数组中获取到的命令行字符串经stoi函数转为整型并赋值给num，num表示要进程池中子进程的个数

    std::vector<Channel> Channels; // 对管道的处理变成了对数组中Channel对象的增删查改，后续创建子进程时应该是一个子进程对应一个管道

    // 创建管道和进程池
    CreatChannelAndSub(num, &Channels);

    TestForProcessPoolAndSub(Channels); // 检测进程池和管道是否创建成功

    // 父进程派发任务

    return 0;
}