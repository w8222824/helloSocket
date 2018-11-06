
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
	EasyTcpClient client;
	//client.InitSocket();
	client.Connect("127.0.0.1", 4567);


	//����UI�߳�
	std::thread t1(cmdThread, &client);            //����һ���߳�ר������ִ�д��εķ���  ����2�ǲ���1������Ҫ�Ĵ���
	t1.detach();									//�̺߳����̷߳���


	while (/*g_bRunt*/client.isRun())								//�����̶߳��������� �����sock���մ����߼�Ҳ���ü���ִ����
	{
		client.OnRun();
	
	}

	client.Close();


	//	// 7.�ر��׽���closesocket
	//#ifdef _WIN32
	//	closesocket(_sock);
	//	//���Windows socket����
	//	WSACleanup();
	//#else
	//	close(_sock);
	//#endif
	printf("���˳����������\n");
	getchar();
	return 0;
}
