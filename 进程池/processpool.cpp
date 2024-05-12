#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Task.hpp"

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

    void CloseChannel() // �ر����ӵ�ǰ�ܵ���wfd
    {
        std::cout << "�رյ�ǰ�������ӵ��ܵ���wfd: " << _wfd << std::endl;
        close(_wfd);
    }

    void wait() // �ӽ��������ȴ�
    {
        pid_t rid = waitpid(_subprocessid, nullptr, 0); // �������ȴ����ӽ���pid��ָ���ӽ��̵��˳���Ϣ����Ϊû��д��ֱ������Ϊ��ָ�룩��ѡ�������ȴ��ķ�ʽ������ֵ�������ɹ����ӽ��̵�pid
        if (rid > 0)
        {
            std::cout << "pid = " << rid << " ���ӽ��̱�Ϊ�����ȴ� " << std::endl; // ��ӡ�����ɹ����ӽ��̵�pid
            std::cout << std::endl;
        }
    }

private:
    int _wfd;
    pid_t _subprocessid;
    std::string _name;
};

// �β������淶
// const & ���ε�Ӧ����һ�������Ͳ���
//& ���ε�Ӧ����һ����������Ͳ���
//* ���ε�Ӧ����һ������Ͳ���

// �����ܵ��ͽ��̳�
void CreatChannelAndSub(int num, std::vector<Channel>* channels, task_t task) // task_t task�ǻص����������ӽ���ִ��forkʱ��ȥ�ص�ָ���õ������ļ��е�work������ʵ���������ļ��ͽ����ļ���Ľ���
{
    for (int i = 0; i < num; i++) // ѭ�������ӽ��̺Ͷ�Ӧ�Ĺܵ�
    {
        // 1�������ܵ�
        int pipefd[2] = { 0 };  // ��Ŷ�д���ļ������������飨��������ÿ��ѭ��ʱ�����ã�
        int n = pipe(pipefd); // ÿ��ѭ��ʱ����OS��������д�������½��ܵ��Ķ��˺�д�˵��ļ���������OS���ݻ�fd��ռ��������䲻ͬ��fd���µĹܵ���
        if (n < 0)
            exit(1); // �����ܵ�ʧ�ܽ����˳�

        // 2�������ӽ���
        pid_t id = fork();
        if (id == 0)
        {
            // ����ڶ��δ����ܵ�ʱ���ӽ����л���ָ���һ���ܵ���rfd
            if (!channels->empty()) // �ܵ����鲻Ϊ�գ������˵ڶ��δ����ܵ�ʱ�Ż�ִ�и��ж����
            {
                for (auto& channel : *channels) // ѭ������֮ǰ�Ĺܵ����õ���Щ�ܵ���rfd��Ȼ��رյ�ǰ���̵���Щrfd
                {
                    channel.CloseChannel();
                }
            }
            std::cout << std::endl;

            sleep(5);

            // �ӽ���
            close(pipefd[1]); // �ر��ӽ��̵�wfd
            // work(pipefd[0]);  // �ӽ��̵ȴ������������ɷ�������

            // dup2(pipefd[0],0);//�ӽ��̲������Դӹܵ��У������Դӱ�׼�����л�ȡ������
            // work();//���ǲ���work��rfd�Ϳ��ԶϾ��ӽ��̴ӹܵ��л�ȡ������,�����ͽ�һ������˹ܵ����ӽ��̼��߼��Ľ���

            dup2(pipefd[0], 0); // �ӽ��̲������Դӹܵ��У������Դӱ�׼�����л�ȡ�����룬���ַ���ʹ���ӽ��̿��������׼����һ���������Թܵ������ݣ��Ӷ�����˴����ͨ���ԺͿ���ֲ�ԡ�
            task();             // ��workҲ��Ϊһ������

            close(pipefd[0]); // �ر��ӽ��̵�rfd
            exit(0);          // �ӽ����˳�
        }

        // 3�������̹����ܵ���
        std::string Channel_name = std::to_string(i) + "�� Channel"; // ÿ��ѭ��i+1���ܵ�����Ϊi�� Channel

        close(pipefd[0]); // �رո����̵�rfd

        // ��������β��ܵ�
        channels->push_back(Channel(pipefd[1], id, Channel_name)); // ����ǰ�ܵ�д�븸���̵�wfd����ǰ�ܵ���Ӧ���ӽ���pid����ǰ�ܵ�
    }
}

// �����̳غ͹ܵ��Ƿ񴴽��ɹ�
void TestForProcessPoolAndSub(std::vector<Channel>& channels)
{
    std::cout << "=========================================================" << std::endl;
    std::cout << "   �ܵ���    " << "   �ܵ���Ӧ���ӽ���pid   " << "   ����ǰ�ܵ�д���wfd   " << std::endl;
    for (auto& Channel : channels)
    {
        std::cout << " " << Channel.GetName() << "          " << Channel.GetProcessID() << "                  " << Channel.GetWfd() << std::endl;
    }
    std::cout << "=========================================================" << std::endl;
}

// ��ȡһ���ܵ��±꣨����ȡģ + static���� ��0~channelnum��ѭ������ʹ��ÿ���ܵ����ᱻʹ�õ�����ѯ������
int NextChannel(int channelnum)
{
    static int next = 0;
    int index = next;
    next++;
    next %= channelnum;
    return index;
}

// ��������
void SendTaskCommand(Channel& channels, int taskcommand) // ��ʱ���������е�һ���ܵ��ɷ����������β�Ӧ����һ���ܵ������͵Ķ���
{
    write(channels.GetWfd(), &taskcommand, sizeof(taskcommand)); // ��ָ����wfd��д�루OS��ͨ����wfd�ҵ���Ӧ�Ĺܵ������ﱾ���Ͼ�����ܵ���д�룩,Ҫд��������룬������Ĵ�С
} // ���ﲻ��Ҫ��ȡ����������ŵ�ַ��ǰ�ĸ��ֽڣ����ǻ�ȡ�����뱾��������

// ���ӽ����ɷ�һ������Ҳ��ͨ���ܵ������ӽ��̣���ΪֻҪ��ĳһ���ܵ����ɷ������ùܵ���Ӧ���ӽ��̾Ϳ��Խ��յ������񣬶��ߵ����ӹ�ϵ��ǰ�Ѿ��������ˣ�
void CtrlProcessOnce(std::vector<Channel>& Channels)
{
    sleep(1);
    // 1��ѡ��һ������(��ȡһ��������,�����ǻ�ȡһ������ָ��)
    int taskcommand = SelectTask();

    // 2��ѡ��һ���ܵ�����������ɷ�
    int channel_index = NextChannel(Channels.size());

    // 3����������
    SendTaskCommand(Channels[channel_index], taskcommand); // ��ָ�ܵ����������룬��Ϊ�ܵ����ӽ��̽����˹�������ܵ�����������ʱ�ӽ������Լ���work�����оͻ��ȡ���ܵ��е������룬Ȼ���ӽ��̾ͻ����ݸ�������ȥִ����Ӧ������

    std::cout << "��������������Ϊ:" << taskcommand << " �ɷ����Ĺܵ���Ϊ: "
        << Channels[channel_index].GetName() << " ����������ӽ���pidΪ: " << Channels[channel_index].GetProcessID() << std::endl;
}

// ���ӽ����ɷ�����
void CtrlProcess(std::vector<Channel>& Channels, int time = -1) // Ĭ��һֱ���ӽ����ɷ�����
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

// ���չܵ����ӽ���(�ͷŶ����ǵȴ�)
void CleanUpChannelAndSubProcess(std::vector<Channel>& Channels)
{
    for (auto& i : Channels)
    {
        i.CloseChannel(); // �ȹر�д��wfd
        i.wait();         // Ȼ�����ӽ��������ȴ�
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

    LoadTask(); // ��������
    std::cout << "��������ɹ�..." << std::endl;

    std::vector<Channel> Channels; // �Թܵ��Ĵ������˶�������Channels�������ɾ���

    // 1�������ܵ��ͽ��̳�
    CreatChannelAndSub(num, &Channels, work); // �涨�ӽ��̴��ۺ��ص�work����

    TestForProcessPoolAndSub(Channels); // �����̳غ͹ܵ��Ƿ񴴽��ɹ������������йܵ����ӽ��̵����ӹ�ϵ�Ѿ�������ɣ�
    std::cout << "�����������ӽ�����ܵ��ɹ�..." << std::endl;
    std::cout << std::endl;

    // 2�����ӽ����ɷ�����
    CtrlProcess(Channels, 4);
    sleep(2);
    std::cout << "�ӽ��̴�������ɹ�..." << std::endl;
    std::cout << std::endl;

    // 3�����չܵ����ӽ���
    CleanUpChannelAndSubProcess(Channels);
    std::cout << "���չܵ����ӽ��̳ɹ�.." << std::endl;
    std::cout << std::endl;
    return 0;
