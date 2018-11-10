
#include<thread>    //c++�߳̿�
#include "EasyTcpClient.hpp"



//cmdThread�̵߳�ǰ�Ƿ���ڵ��ж���
bool g_bRunt = true;

//����һ������CMD����ķ���
void cmdThread(EasyTcpClient* client)
{

	while (true)
	{
		char cmdBuf[256] = {};    //����һ���ռ��СΪ256�ַ�����

		scanf("%s", cmdBuf);        //����̨�����ַ���
		if (0 == strcmp(cmdBuf, "exit"))    //���������ַ���Ϊexit
		{
			client->Close();
			printf("�˳�cmdThread�߳�\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.PassWord, "lydmm");
			client->SendData(&login);

		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout;
			strcpy(logout.userName, "lyd");
			client->SendData(&logout);
		}
		else
		{
			printf("��֧�ֵ����\n");
		}
	}


}//void cmdThread(SOCKET _Sock)




int main() {
	EasyTcpClient client1;
	//client.InitSocket();
	client1.Connect("192.168.81.1", 4567);		//windows�µ�ipv4  ���ص�ַ

	EasyTcpClient client2;						
	//client.InitSocket();
	client2.Connect("192.168.236.133", 4567);	//192.168.236.133   ubuntu�µı��ص�ַ


	EasyTcpClient client3;
	//client.InitSocket();
	client3.Connect("192.168.236.130", 4567);		////192.168.236.130   macos�µı��ص�ַ

	//����UI�߳�
	std::thread t1(cmdThread, &client1);            //����һ���߳�ר������ִ�д��εķ���  ����2�ǲ���1������Ҫ�Ĵ���
	t1.detach();									//�̺߳����̷߳���
	std::thread t2(cmdThread, &client2);            //����һ���߳�ר������ִ�д��εķ���  ����2�ǲ���1������Ҫ�Ĵ���
	t2.detach();									//�̺߳����̷߳���
	std::thread t3(cmdThread, &client2);            //����һ���߳�ר������ִ�д��εķ���  ����2�ǲ���1������Ҫ�Ĵ���
	t3.detach();									//�̺߳����̷߳���

	while (/*g_bRunt*/client1.isRun()|| client2.isRun()|| client3.isRun())								//�����̶߳��������� �����sock���մ����߼�Ҳ���ü���ִ����
	{
		client1.OnRun();
		client2.OnRun();
		client3.OnRun();
	}

	client1.Close();
	client2.Close();
	client3.Close();

	printf("���˳����������\n");
	getchar();
	return 0;
}
