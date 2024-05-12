#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Task.hpp"

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

    void CloseChannel() // 关闭连接当前管道的wfd
    {
        std::cout << "关闭当前进程连接到管道的wfd: " << _wfd << std::endl;
        close(_wfd);
    }

    void wait() // 子进程阻塞等待
    {
        pid_t rid = waitpid(_subprocessid, nullptr, 0); // （阻塞等待的子进程pid，指向子进程的退出信息（因为没有写就直接设置为空指针），选择阻塞等待的方式）返回值是阻塞成功的子进程的pid
        if (rid > 0)
        {
            std::cout << "pid = " << rid << " 的子进程变为阻塞等待 " << std::endl; // 打印阻塞成功的子进程的pid
            std::cout << std::endl;
        }
    }

private:
    int _wfd;
    pid_t _subprocessid;
    std::string _name;
};

// 形参命名规范
// const & 修饰的应该是一个输入型参数
//& 修饰的应该是一个输入输出型参数
//* 修饰的应该是一个输出型参数

// 创建管道和进程池
void CreatChannelAndSub(int num, std::vector<Channel>* channels, task_t task) // task_t task是回调函数，当子进程执行fork时会去回调指定好的任务文件中的work函数，实现了任务文件和进程文件间的解耦
{
    for (int i = 0; i < num; i++) // 循环创建子进程和对应的管道
    {
        // 1、创建管道
        int pipefd[2] = { 0 };  // 存放读写端文件描述符的数组（该数组在每次循环时都重置）
        int n = pipe(pipefd); // 每次循环时都由OS向数组中写入分配给新建管道的读端和写端的文件描述符（OS依据会fd的占用情况分配不同的fd给新的管道）
        if (n < 0)
            exit(1); // 创建管道失败进程退出

        // 2、创建子进程
        pid_t id = fork();
        if (id == 0)
        {
            // 处理第二次创建管道时的子进程中还有指向第一个管道的rfd
            if (!channels->empty()) // 管道数组不为空，即到了第二次创建管道时才会执行该判断语句
            {
                for (auto& channel : *channels) // 循环遍历之前的管道并拿到这些管道的rfd，然后关闭当前进程的这些rfd
                {
                    channel.CloseChannel();
                }
            }
            std::cout << std::endl;

            sleep(5);

            // 子进程
            close(pipefd[1]); // 关闭子进程的wfd
            // work(pipefd[0]);  // 子进程等待并处理父进程派发的任务

            // dup2(pipefd[0],0);//子进程不仅可以从管道中，还可以从标准输入中获取任务码
            // work();//我们不给work传rfd就可以断绝子进程从管道中获取任务码,这样就进一步完成了管道和子进程间逻辑的解耦

            dup2(pipefd[0], 0); // 子进程不仅可以从管道中，还可以从标准输入中获取任务码，这种方法使得子进程可以像处理标准输入一样处理来自管道的数据，从而提高了代码的通用性和可移植性。
            task();             // 将work也视为一个任务

            close(pipefd[0]); // 关闭子进程的rfd
            exit(0);          // 子进程退出
        }

        // 3、父进程构建管道名
        std::string Channel_name = std::to_string(i) + "号 Channel"; // 每次循环i+1，管道名即为i号 Channel

        close(pipefd[0]); // 关闭父进程的rfd

        // 向数组中尾插管道
        channels->push_back(Channel(pipefd[1], id, Channel_name)); // 会向当前管道写入父进程的wfd，当前管道对应的子进程pid，当前管道
    }
}

// 检测进程池和管道是否创建成功
void TestForProcessPoolAndSub(std::vector<Channel>& channels)
{
    std::cout << "=========================================================" << std::endl;
    std::cout << "   管道名    " << "   管道对应的子进程pid   " << "   会向当前管道写入的wfd   " << std::endl;
    for (auto& Channel : channels)
    {
        std::cout << " " << Channel.GetName() << "          " << Channel.GetProcessID() << "                  " << Channel.GetWfd() << std::endl;
    }
    std::cout << "=========================================================" << std::endl;
}

// 获取一个管道下标（利用取模 + static变量 在0~channelnum间循环）（使得每个管道都会被使用到的轮询方案）
int NextChannel(int channelnum)
{
    static int next = 0;
    int index = next;
    next++;
    next %= channelnum;
    return index;
}

// 发送任务
void SendTaskCommand(Channel& channels, int taskcommand) // 此时是向数组中的一个管道派发任务，所以形参应该是一个管道类类型的对象
{
    write(channels.GetWfd(), &taskcommand, sizeof(taskcommand)); // 像指定的wfd中写入（OS会通过该wfd找到对应的管道，这里本质上就是向管道中写入）,要写入的任务码，任务码的大小
} // 这里不是要截取任务码所存放地址的前四个字节，而是获取任务码本身，别搞错了

// 向子进程派发一次任务（也叫通过管道控制子进程，因为只要向某一个管道中派发任务后该管道对应的子进程就可以接收到该任务，二者的连接关系提前已经建立好了）
void CtrlProcessOnce(std::vector<Channel>& Channels)
{
    sleep(1);
    // 1、选择一个任务(获取一个任务码,本质是获取一个函数指针)
    int taskcommand = SelectTask();

    // 2、选择一个管道进行任务的派发
    int channel_index = NextChannel(Channels.size());

    // 3、发送任务
    SendTaskCommand(Channels[channel_index], taskcommand); // 向指管道发送任务码，因为管道和子进程建立了关联，向管道中输入内容时子进程在自己的work函数中就会读取到管道中的任务码，然后子进程就会依据该任务码去执行相应的任务

    std::cout << "分配的随机任务码为:" << taskcommand << " 派发给的管道名为: "
        << Channels[channel_index].GetName() << " 处理任务的子进程pid为: " << Channels[channel_index].GetProcessID() << std::endl;
}

// 向子进程派发任务
void CtrlProcess(std::vector<Channel>& Channels, int time = -1) // 默认一直向子进程派发任务
{
    if (time > 0)
    {
        while (time--)
        {
            CtrlProcessOnce(Channels);
        }
    }
    else
    {
        while (true)
        {
            CtrlProcessOnce(Channels);
        }
    }
}

// 回收管道和子进程(释放而不是等待)
void CleanUpChannelAndSubProcess(std::vector<Channel>& Channels)
{
    for (auto& i : Channels)
    {
        i.CloseChannel(); // 先关闭写端wfd
        i.wait();         // 然后让子进程阻塞等待
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

    LoadTask(); // 加载任务
    std::cout << "加载任务成功..." << std::endl;

    std::vector<Channel> Channels; // 对管道的处理变成了对数组中Channels对象的增删查改

    // 1、创建管道和进程池
    CreatChannelAndSub(num, &Channels, work); // 规定子进程创价后会回调work函数

    TestForProcessPoolAndSub(Channels); // 检测进程池和管道是否创建成功（到这里所有管道和子进程的连接关系已经建立完成）
    std::cout << "创建并关联子进程与管道成功..." << std::endl;
    std::cout << std::endl;

    // 2、向子进程派发任务
    CtrlProcess(Channels, 4);
    sleep(2);
    std::cout << "子进程处理任务成功..." << std::endl;
    std::cout << std::endl;

    // 3、回收管道和子进程
    CleanUpChannelAndSubProcess(Channels);
    std::cout << "回收管道和子进程成功.." << std::endl;
    std::cout << std::endl;
    return 0;
