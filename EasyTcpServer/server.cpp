

#include "EasyTcpServer.hpp"
#include <thread>

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

	server.Listen(50);			//���� ��ʾ������5���ͻ���
	

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
