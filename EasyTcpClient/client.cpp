
#include<thread>    //c++�߳̿�
#include "EasyTcpClient.hpp"



//cmdThread�̵߳�ǰ�Ƿ���ڵ��ж���
bool g_bRunt = true;

//����һ������CMD����ķ���
void cmdThread(/*EasyTcpClient* client*/)
{

	while (true)
	{

		char cmdBuf[256] = {};    //����һ���ռ��СΪ256�ַ�����

		scanf("%s", cmdBuf);        //����̨�����ַ���
		if (0 == strcmp(cmdBuf, "exit"))    //���������ַ���Ϊexit
		{
			//client->Close();
			 g_bRunt = false;
			printf("�˳�cmdThread�߳�\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.PassWord, "lydmm");
			//client->SendData(&login);

		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout;
			strcpy(logout.userName, "lyd");
			//client->SendData(&logout);
		}
		else
		{
			printf("��֧�ֵ����\n");
		}
	}


}//void cmdThread(SOCKET _Sock)




int main() {
	//const int  cCount = FD_SETSIZE-1;		//FD_SETSIZE��������� 64   -1 ��ʾ���������Ǹ�
		const int  cCount = 1;		//FD_SETSIZE��������� 64   -1 ��ʾ���������Ǹ�
	EasyTcpClient* client[cCount];			//����һ������  ��СΪ10		
	int  a = sizeof(EasyTcpClient);
	for (int i = 0; i < cCount; i++)
	{
	//	client[i].Connect("47.98.215.178", 4567);				//��������ͻ������ӷ����   ������  ����ip
		client[i] = new EasyTcpClient();
	//	client[i]->Connect("127.0.0.1", 4567);				// ���ص�ַ
		client[i]->Connect("47.98.215.178", 4567);		
		//client[i]->Connect("192.168.158.155", 4567);		//ubuntu�µ�ipv4 ���ص�ַ
	}

	//client.InitSocket();
	//client1.Connect("172.16.47.190", 4567);				// ������  ˽��ip
	//client1.Connect("47.98.215.178", 4567);				// ������  ����ip
	//client1.Connect("127.0.0.1", 4567);				// ���ص�ַ
	//client1.Connect("192.168.81.1", 4567);		//windows�µ�ipv4  ���ص�ַ
//	client1.Connect("192.168.236.130", 4567);		//mac�µ�ipv4  ���ص�ַ
	//client1.Connect("192.168.236.133", 4567);		//ubuntu�µ�ipv4 ���ص�ַ

	//����UI�߳�
	std::thread t1(cmdThread/*, &client1*/);            //����һ���߳�ר������ִ�д��εķ���  ����2�ǲ���1������Ҫ�Ĵ���
		t1.detach();									//�̺߳����̷߳���



	Login login;
	strcpy(login.userName, "lyd");
	strcpy(login.PassWord, "lydmm");
	while (/*client1.isRun()&& */g_bRunt)								//�����̶߳��������� �����sock���մ����߼�Ҳ���ü���ִ����
	{
		//client1.OnRun();			��ʱע�� ������������
		//client1.SendData(&login);
		for (int i = 0; i < cCount; i++)
		{
					
			client[i]->SendData(&login);			//ѭ����������
			client[i]->OnRun();					//����
		}
	}

	for (int i = 0; i < cCount; i++)
	{
		client[i]->Close();
	}
	//client1.Close();


	printf("���˳����������\n");
	getchar();
	return 0;
}
