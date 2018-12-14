
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


//�ͻ������ӵ��������
const int  cCount = 10000;		//�Ѿ���EasyTcpServer���潫WINSOCKE2.H�����FD_SETSIZE��64�Զ������1024
								//	const int  cCount = 1;		//FD_SETSIZE��������� 64   -1 ��ʾ���������Ǹ�
//�����߳�����
const int  tCount = 4;

//����һ��������Ϊ�ͻ�������  ��СΪ10		
EasyTcpClient* client[cCount];			

//���巢���߳�
void sendThread(int id)
{
	//Ŀǰ�����߳� ��4�� ID���� 1~4
	int c = cCount / tCount;	//ÿ�������߳�Ҫ���ӵĿͻ�������
	int begin = (id-1)*c;		//������ÿ���߳����ӵĿͻ���������￪ʼ
	int  end = id * c;			//������ÿ���߳����ӵĿͻ��������Ľ���

	for (size_t i = begin; i < end; i++)
	{
		client[i] = new EasyTcpClient();
	}

	for (size_t i = begin; i < end; i++)
	{

		client[i]->Connect("127.0.0.1", 4567);			//ȫ���������˺��ٿ�ʼ����
		printf("thread<%d> Connect=%d\n",id, i);


	}

	std::chrono::milliseconds t(5000);// ����  c++��׼�������  ʱ����� tΪ 3000����   
	std::this_thread::sleep_for(t);// ����һ���߳� ����t��   ��ƽ̨�� ���



	//client.InitSocket();
	//client1.Connect("172.16.47.190", 4567);				// ������  ˽��ip
	//client1.Connect("47.98.215.178", 4567);				// ������  ����ip
	//client1.Connect("127.0.0.1", 4567);				// ���ص�ַ
	//client1.Connect("192.168.81.1", 4567);		//windows�µ�ipv4  ���ص�ַ
	//	client1.Connect("192.168.236.130", 4567);		//mac�µ�ipv4  ���ص�ַ
	//client1.Connect("192.168.236.133", 4567);		//ubuntu�µ�ipv4 ���ص�ַ

	Login login[10];
	for (size_t i = 0; i < 10; i++)
	{
		strcpy(login[i].userName, "lyd");
		strcpy(login[i].PassWord, "lydmm");
	}
	const int nLen = sizeof(login);
	while (/*client1.isRun()&& */g_bRunt)								//�����̶߳��������� �����sock���մ����߼�Ҳ���ü���ִ����
	{
		for (int i = 0; i < cCount; i++)
		{
			client[i]->SendData(&login[i], nLen);			//ѭ����������
		//	client[i]->OnRun();					//����
		}
	}

	for (int i = 0; i < cCount; i++)
	{
		client[i]->Close();
	}
}


int main() {

		//����UI�߳�
		std::thread t1(cmdThread/*, &client1*/);            //����һ���߳�ר������ִ�д��εķ���  ����2�ǲ���1������Ҫ�Ĵ���
		t1.detach();										//�̺߳����̷߳���
		
		//���������߳�
		for (size_t i = 0; i < tCount; i++)
		{
			std::thread t1(sendThread,i+1);
			t1.detach();//�̺߳����̷߳���
		}



		while (g_bRunt)
		{
			Sleep(100);
		}




	printf("���˳����������\n");
	getchar();
	return 0;
}
