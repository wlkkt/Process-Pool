#include "task.hpp"

#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>

 // �ܵ�
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

    // ��ȡд�˵�wfd��Ŀ���ӽ��̵�pid����ǰ�ܵ���
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

// ���˲���
void work(int rfd)
{
    // �ӽ���ѭ���ȴ�
    while (true)
    {
        sleep(1);
    }
}

// �β������淶
// const & ���ε�Ӧ����һ�������Ͳ���
//& ���ε�Ӧ����һ����������Ͳ���
//* ���ε�Ӧ����һ������Ͳ���

// �����ܵ��ͽ��̳�
void CreatChannelAndSub(int num, std::vector<Channel>* channels)
{
    for (int i = 0; i < num; i++) // ѭ�������ӽ��̺Ͷ�Ӧ�Ĺܵ�
    {
        // 1�������ܵ�
        int pipefd[2] = { 0 };
        int n = pipe(pipefd); // ���մ����ܵ������ķ���ֵ,������pipe������OS��������д���ļ���������������Ԫ��OS�������rfd���ڶ���Ԫ����OS�����wfd
        if (n < 0)
            exit(1); // �����ܵ�ʧ�ܽ����˳�

        // 2�������ӽ���
        pid_t id = fork();
        if (id == 0)
        {
            // �ӽ���----����
            close(pipefd[1]); // �ر��ӽ��̵�wfd
            work(pipefd[0]);  // �ӽ��̴����ȡ�ܵ��е�����
            close(pipefd[0]); // �ر��ӽ��̵�rfd
            exit(0);          // �ӽ����˳�
        }
        // 3�������̹����ܵ���
        std::string Channel_name = "Channel-" + std::to_string(i); // ÿ��ѭ��i+1���ܵ�����ΪChannel-i

        // ������----д��
        close(pipefd[0]); // �رո����̵�rfd

        // ��������Ҫ���߹ܵ���д�˵Ĺܵ���wfd���ӽ��̵�pid,�ܵ�������
        channels->push_back(Channel(pipefd[1], id, Channel_name));
    }
}

// �����̳غ͹ܵ��Ƿ񴴽��ɹ�
void TestForProcessPoolAndSub(const std::vector<Channel>& channels)
{

    for (auto& Channel : channels)
    {
        std::cout << "=======================================================" << std::endl;
        std::cout << Channel.GetName() << "-" << Channel.GetProcessID() << "-" << Channel.GetWfd() << std::endl;
    }
}

// ��������������̵Ľ��̳أ����������е��������ַ�����./processpool 5,һ�����������ַ���������Ӧ��Ϊ2ʱ���ܽ��д������̳�
int main(int agrc, char* argv[])
{

    if (agrc != 2) // �����в�����Ϊ2��ô�ͱ�������
    {
        std::cerr << "Usage: " << argv[0] << " processnum" << std::endl;
        return 1;
    }

    int num = std::stoi(argv[1]); // ��argv�����л�ȡ�����������ַ�����stoi����תΪ���Ͳ���ֵ��num��num��ʾҪ���̳����ӽ��̵ĸ���

    std::vector<Channel> Channels; // �Թܵ��Ĵ������˶�������Channel�������ɾ��ģ����������ӽ���ʱӦ����һ���ӽ��̶�Ӧһ���ܵ�

    // �����ܵ��ͽ��̳�
    CreatChannelAndSub(num, &Channels);

    TestForProcessPoolAndSub(Channels); // �����̳غ͹ܵ��Ƿ񴴽��ɹ�

    // �������ɷ�����

    return 0;
}