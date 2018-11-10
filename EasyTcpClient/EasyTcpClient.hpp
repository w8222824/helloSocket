#ifndef _EasyTcpClient_hpp_					//���û�ж���������δ��� ������  
#define _EasyTcpClient_hpp_					//��ôд��ֻ֤�ᱻ����һ��

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
#include <stdio.h>							//printf��Ҫ�����ͷ�ļ�
#include "MessageHeader.hpp"

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
		Close();
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
			printf("<socket=%d>��ʼ������socket����Ч�ģ���ʼ�رվ����ӡ�\n", _sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//��������   IPv4  ��������  Э�鴫�����tcp
		if (INVALID_SOCKET == _sock)        //�����׽���socketʧ��
		{
			printf("���󣬽���Socketʧ�ܡ�\n");
		}
		else
		{
			printf("��ȷ������Socket=<%d>�ɹ���\n",_sock);
		}
		return 0;
	}

	//���ӷ����� ����1 ip  ����2  �˿ں�
	int Connect(const char* ip,unsigned short port)
	{
		if (INVALID_SOCKET == _sock)		//������ӷ�������ʱ��socket����Ч�� ���ǳ�ʼ��socketһ��
		{
			InitSocket();
		}
		// 2.���ӷ�����connect
		 sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;            //Э��� һ����AF_INET��ʾTCP/IPЭ�顣  �����ʾipv4
		_sin.sin_port = htons(port);		 //Ϊʲô����4567����������  ��Ϊ����Ҫת�� host to net unsigned short����������������ת����������޷��ŵĶ��ַ����������ͣ�  �ͻ��˱�ʾ���ӵ�����˵�4567�˿ں�
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);//sin_addr��һ�������壬��������Ϳ���ʹ�ö��ַ�ʽ��ʾIP��ַ��һ������ʹ�����е����һ�ַ�ʽ�������޷��ų�������������ʾIP��ַ����ʱ���õ���s_nu���ݳ�Ա��s_un��һ���ṹ�壬����ֻ��һ�����ݳ�Ա����s_addr��           inet_addr("127.0.0.1");    //"127.0.0.1"�����ĵ�ַ ��ôд���ܷ�ֹ�����ķ���,  INADDR_ANY��ʾ������ȫ��ip��ַ�����Է���
#else
		_sin.sin_addr.s_addr = inet_addr(ip);
#endif
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));    //���Ӷ�Ӧ��socket    ����3���Ȼ��Ǵ����ͱȽϺ�
		if (SOCKET_ERROR == ret)				//�����׽���socketʧ��
		{
			printf("<socket=%d>�������ӷ�����<%s:%d>ʧ�ܡ�\n",_sock,ip,port);
		}
		else {
			printf("<socket=%d>��ȷ�����ӷ�����<%s:%d>�ɹ���\n", _sock, ip, port);
		}
		return ret;
	}

	//�ر��׽���  closesocket
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
	bool  OnRun() {


		if (isRun())			//�ж�socket�Ƿ�������
		{
			fd_set fdReads;		//������һ���ɶ���������(socket)����  
			FD_ZERO(&fdReads);			//Ȼ�������������������
			FD_SET(_sock, &fdReads);	//������1_sock��ӵ�����ɶ���������������

			timeval t = { 0,0 };		//����һ�� ʱ�������  �����ʾ1��   ��ʾsocket��ô���һ��
			int ret = select(_sock + 1, &fdReads, 0, 0, &t);		//������һ��socket���� ����1���socket���ֵ+1  ����2�ǿɶ����׽��ֽ��  ����3�ǿ�д���׽��ּ��ϣ�����Ҫ��0Ĭ��  ����4�쳣���׽��ּ��� ����Ҫ  ��0  ����5 ÿ�μ��ʱ��  ������1��
			if (ret < 0)	//С��0һ��������ʧ����  
			{
				printf("<socket=%d>select�������1\n", _sock);
				Close();
				return false;
			}

			if (FD_ISSET(_sock, &fdReads))		//�ж����sock  �Ƿ��ڿɶ�����������������
			{
				FD_CLR(_sock, &fdReads);	//����һ��  �������ܼ���ֻ�������˼����� û�������Ӧ������

				if (-1 == RecvData(_sock))		//���ӶϿ�
				{
					printf("<socket=%d>select�������2\n", _sock);
					Close();
					return false;
				}
			}
			return true;				//����true  ��ʾǰ��Ķ�û������ �������ӳɹ�  Ҳ�ɹ��ͷ���������û�жϿ� 
		}
		return false;			//ȱ�ٵ�
	}//OnRun()

	 //�Ƿ���������   true��ʾ���� ����û������
	bool  isRun() {

		return _sock != INVALID_SOCKET;
	}


	//��������	����ճ�� ��� 				����һ���������Ѵ������˵��߼����Ž���
	int RecvData(SOCKET _cSock) {

		char szRecv[4096] = {};       //����һ�������� ����������  recv���յ��������߷ŵ��������������
									   //5.���շ���˵�����
		int nLen = (int)recv(_cSock, szRecv, sizeof(DataHeader), 0);    //���շ���˷��͹���������  ����1��Ӧ�ͻ��˵�socket  ����2 ���տͻ��˷������ݵĻ�����  ����3 ��������С  ����4  0Ĭ��
		DataHeader* header = (DataHeader*)szRecv;        //��ÿ�ν��յ������ݸ����header
		if (nLen <= 0)
		{
			printf("�������<_Sock%d>�Ͽ�����,���������\n", _cSock);
			return -1;
		}

		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);    //���հ�ͷ��CMD_LOGIN_RESULT������

		OnNetMsg(header);
		return 0;
	}//int processor(SOCKET _Sock)


	 //��Ӧ������Ϣ
	void OnNetMsg(DataHeader* header) {


		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResult* login = (LoginResult*)header;        //Logout���ݽṹ���� ���ǽ��յ�������
			 //	printf("�յ������<_Sock%d>����CMD_LOGIN_RESULT  ���ݳ���:%d ���صĽ��:%d\n", _sock, loginResult->dataLength, loginResult->result);
			printf("<socket=%d>�յ��������Ϣ��CMD_LOGIN_RESULT,���ݳ��ȣ�%d\n",_sock, login->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			LogoutResult* logout = (LogoutResult*)header;     //Logout���ݽṹ���� ���ǽ��յ�������
			//	printf("�յ������<_Sock%d>����CMD_LOGOUT_RESULT  ���ݳ���:%d ���صĽ��:%d\n", _sock, logoutResult->dataLength, logoutResult->result);
			printf("<socket=%d>�յ��������Ϣ��CMD_LOGOUT_RESULT,���ݳ��ȣ�%d\n", _sock, logout->dataLength);

		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* userJoin = (NewUserJoin*)header;        //Logout���ݽṹ���� ���ǽ��յ�������
			 //	printf("�յ������<_Sock%d>����CMD_NEW_USER_JOIN  ���ݳ���:%d �¼���Ŀͻ���socket:%d\n", _sock, newUserJoin->dataLength, newUserJoin->socket);
			printf("<socket=%d>�յ��������Ϣ��CMD_NEW_USER_JOIN,���ݳ��ȣ�%d\n", _sock, userJoin->dataLength);

		}
		break;
		}
	}//OnNetMsg

	 //��������
	int SendData(DataHeader* header) {

		if (isRun() && header)	//socket�Ƿ�������  header��Ϊ��
		{
			return send(_sock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}


private:

};

#else
//�Ѿ����������
#endif