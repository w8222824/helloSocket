//���⻥������������  windows������һЩsocke����������  û������ܻᵼ�º궨���ͻ


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")        // û����仰 ִ�е�WSAStartup(ver, &dat);�ᱨ�޷����� �޷��������ⲿ���� __imp__WSAStartup@8�Ĵ���   ��仰����˼������Ҫһ����̬���ӿ�  ���ֻ����windows�²����� ��������Ҫ��ƽ̨ ���������Ҽ���Ŀ���Ե�������� �ڵ������ �ڸ����������������ws2_32.lib

#else
#include <unistd.h> //uni std  ���ע�ͱ�ʾunix ϵͳ��  �ı�׼�����˼  windows��linuxϵͳ���ǻ���unixϵͳ������
#include <arpa/inet.h>//unixϵͳ�°�����SOCK�Ŀ� ��ӦWINSOCK2��H
#include <string.h>
#define SOCKET int    //����linuxϵͳ�µ�socket
//����2�����Ƕ���linuxϵͳ�µ�INVALID_SOCKET�����ֵ���ж�socket�Ƿ���Ч�ģ�
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

#include <stdio.h>    //printf��Ҫ�����ͷ�ļ�
#include<thread>    //c++�߳̿�
#include <vector>    //��������  �����洢

#pragma comment(lib,"ws2_32.lib")        // û����仰 ִ�е�WSAStartup(ver, &dat);�ᱨ�޷����� �޷��������ⲿ���� __imp__WSAStartup@8�Ĵ���   ��仰����˼������Ҫһ����̬���ӿ�  ���ֻ����windows�²����� ��������Ҫ��ƽ̨ ���������Ҽ���Ŀ���Ե�������� �ڵ������ �ڸ����������������ws2_32.lib


#include "EasyTcpServer.hpp"


bool  g_bRun = true;			//���Ʒ������Ƿ��˳���״̬��
//����һ������CMD����ķ���
void cmdThread()
{

	while (true)
	{

		char cmdBuf[256] = {};    //����һ���ռ��СΪ256�ַ�����

		scanf("%s", cmdBuf);        //����̨�����ַ���
		if (0 == strcmp(cmdBuf, "exit"))    //���������ַ���Ϊexit
		{	
			
	
			g_bRun = false;
			printf("�˳�cmdThread�߳�\n");
			break;
		}
		else
		{
			printf("��֧�ֵ����\n");
		}
	}


}//void cmdThread(SOCKET _Sock)




int main() {

	EasyTcpServer server;		//����
	server.InitSocket();		//��ʼ��socket ���Ǵ���socket
	server.Bind(nullptr,4567);	//������ip�� 4567�˿ں�

	server.Listen(5);			//���� ��ʾ������5���ͻ���
	

								//����UI�߳�
	std::thread t1(cmdThread);            //����һ���߳�ר������ִ�д��εķ���  ����2�ǲ���1������Ҫ�Ĵ���
	t1.detach();									//�̺߳����̷߳���



	while (server.isRun()&& g_bRun)        //�жϷ�����socket�Ƿ����  �� g_bRunΪtrue 
	{
		server.OnRun();		//������յ�����Ϣ
		
	}

	server.Close();

	printf("���˳����������\n");
	getchar();

	return 0;
}
