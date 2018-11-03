#ifndef _EasyTcpClient.hpp_					//���û�ж���������δ��� ������  
#define _EasyTcpClient.hpp_					//��ôд��ֻ֤�ᱻ����һ��

#ifdef _WIN32

	#define WIN32_LEAN_AND_MEAN				//���⻥������������  windows������һЩsocke����������  û������ܻᵼ�º궨���ͻ
	#include <windows.h>
	#include <WinSock2.h>
	#pragma comment(lib,"ws2_32.lib")		// û����仰 ִ�е�WSAStartup(ver, &dat);�ᱨ�޷����� �޷��������ⲿ���� __imp__WSAStartup@8�Ĵ���   ��仰����˼������Ҫһ����̬���ӿ�  ���ֻ����windows�²����� ��������Ҫ��ƽ̨ ���������Ҽ���Ŀ���Ե�������� �ڵ������ �ڸ����������������ws2_32.lib

#else
	#include <unistd.h>						//uni std  ���ע�ͱ�ʾunix ϵͳ��  �ı�׼�����˼  windows��linuxϵͳ���ǻ���unixϵͳ������
	#include <arpa/inet.h>					//unixϵͳ�°�����SOCK�Ŀ� ��ӦWINSOCK2��H
	#include <string.h>
	#define SOCKET int						//����linuxϵͳ�µ�socket
											//����2�����Ƕ���linuxϵͳ�µ�INVALID_SOCKET�����ֵ���ж�socket�Ƿ���Ч�ģ�
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif

#include <stdio.h>    //printf��Ҫ�����ͷ�ļ�

class EasyTcpClient
{
	SOCKET _sock;					//����һ��socket  �׽���
public:
	//���캯��
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;//����һ����Ч��socket  

	}


	//����������
	virtual ~EasyTcpClient()
	{
		closesocket(_sock);
	};

	//��ʼ��socket
	int InitSocket() 
	{
															//����Win Sock 2.x����
#ifdef _WIN32
															//����Windows socket 2.x����
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		//--��Socket API��������TCP�ͻ���
		// 1.����һ��socket �׽���    ��ô�����׽���socket����  ����1��ʾipv4���͵�(ipv6��AF_INET6)  ������ʲô�������͵�sock  �����������͵�   ������������Ҫ����ʲô���͵�socket ������tcp���͵�  ��������һ���׽���

		if (INVALID_SOCKET != _sock)		//�����ʱsocket����Ч�� �����ʼ��ǰ ��Ӧ��socket������Ч�� ����Ҫ�ȹرյ�
		{
			printf("<socket=%d>��ʼ������socket����Ч�ģ���ʼ�رվ����ӡ�\n",_sock);
			Close();	
		}
		
		
		_sock = socket(AF_INET, SOCK_STREAM, /*0*/IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)        //�����׽���socketʧ��
		{
			printf("���󣬽���Socketʧ�ܡ�\n");
		}
		else 
		{
			printf("��ȷ������Socket�ɹ���\n");
		}

	}

	//���ӷ�����
	int Connect(char* ip, unsigned short port) 
	{
		if (INVALID_SOCKET == _sock)		//������ӷ�������ʱ��socket����Ч�� ���ǳ�ʼ��socketһ��
		{
			InitSocket();
		}
		// 2.���ӷ�����connect
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;            //Э��� һ����AF_INET��ʾTCP/IPЭ�顣  �����ʾipv4
		_sin.sin_port = htons(4567);		 //Ϊʲô����4567����������  ��Ϊ����Ҫת�� host to net unsigned short����������������ת����������޷��ŵĶ��ַ����������ͣ�  �ͻ��˱�ʾ���ӵ�����˵�4567�˿ں�
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);//sin_addr��һ�������壬��������Ϳ���ʹ�ö��ַ�ʽ��ʾIP��ַ��һ������ʹ�����е����һ�ַ�ʽ�������޷��ų�������������ʾIP��ַ����ʱ���õ���s_nu���ݳ�Ա��s_un��һ���ṹ�壬����ֻ��һ�����ݳ�Ա����s_addr��           inet_addr("127.0.0.1");    //"127.0.0.1"�����ĵ�ַ ��ôд���ܷ�ֹ�����ķ���,  INADDR_ANY��ʾ������ȫ��ip��ַ�����Է���
#else
		_sin.sin_addr.s_addr = inet_addr(ip);
#endif
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));    //���Ӷ�Ӧ��socket    ����3���Ȼ��Ǵ����ͱȽϺ�
		if (SOCKET_ERROR == ret)				//�����׽���socketʧ��
		{
			printf("�������ӷ�����ʧ��ʧ�ܡ�\n");
		}
		else {
			printf("��ȷ�����ӷ������ɹ���\n");
		}
		return ret;
	}

	//�ر�socket
	void Close()
	{
		//���socket������Ч�� ��ִ�������
		if (_sock != INVALID_SOCKET) 
		{
			//�ر��׽���closesocket
#ifdef _WIN32
			closesocket(_sock);
			//���Windows socket����
			WSACleanup();
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;//�ر�������Ϻ� ������Чֵ
		}
	


	}


	//��������
	//��������
	//����������Ϣ

private:

};

#else
		//�Ѿ����������
#endif