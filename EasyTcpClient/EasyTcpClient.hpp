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
		WSAStartup(ver, &dat);	//�ҵıȽ���ϸ��    https://blog.csdn.net/clemontine/article/details/53141041 
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
		printf("��ȷ������Socket=<%d>�ɹ���\n", _sock);
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
			printf("<socket=%d>�������ӷ�����<%s:%d>ʧ��.\n",_sock,ip,port);
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
	int _nCount = 0;
	bool  OnRun() {


		if (isRun())			//�ж�socket�Ƿ�������
		{
			fd_set fdReads;		//������һ���ɶ���������(socket)����  
			FD_ZERO(&fdReads);			//Ȼ�������������������
			FD_SET(_sock, &fdReads);	//������1_sock��ӵ�����ɶ���������������

			timeval t = { 0,0 };		//����һ�� ʱ�������  �����ʾ1��   ��ʾsocket��ô���һ��
			int ret = select(_sock + 1, &fdReads, 0, 0, &t);		//������һ��socket���� ����1���socket���ֵ+1  ����2�ǿɶ����׽��ֽ��  ����3�ǿ�д���׽��ּ��ϣ�����Ҫ��0Ĭ��  ����4�쳣���׽��ּ��� ����Ҫ  ��0  ����5 ÿ�μ��ʱ��  ������1��
			//printf("select ret=%d count=%d\n", ret, _nCount++);
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

#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240			// ʹ�ú궨��һ���ַ�����������С��Ԫ��С  ����RECV_BUFF_SIZE��ʾ����10240  10Kb��СӦ��
#endif
	
	char _szRecv[RECV_BUFF_SIZE] = {};				//����һ�����ջ����� ����������  recv���յ��������߷ŵ��������������,�������������������һ�δ�ϵͳ�Ļ�������ˮ��ˮưһ��
	char _szMsgBuf[RECV_BUFF_SIZE * 10] = {};		//�ڶ�������  ��Ϣ������
	int _lastPos = 0;								//��Ϣ����������β����λ��
	//��������	����ճ�� ��� 				����һ���������Ѵ������˵��߼����Ž���
	int RecvData(SOCKET _cSock) {


		//5.���շ���˵�����
		int nLen = (int)recv(_cSock, _szRecv, /*sizeof(DataHeader)*/409600, 0);    //���շ���˷��͹���������  ����1��Ӧ�ͻ��˵�socket  ����2 ���տͻ��˷������ݵĻ�����  ����3 ��������С  ����4  0Ĭ��
		printf("nLen=%d\n", nLen);
		
		if (nLen <= 0)
		{
			printf("�������<_Sock%d>�Ͽ�����,���������\n", _cSock);
			return -1;
		}
		//����ȡ�������ݿ�������Ϣ������
		memcpy(_szMsgBuf+ _lastPos, _szRecv, nLen);				//��ÿ�ν������ݷŵ����Ƕ���ĵڶ�������_szMsgBuf���棨����_szRecv���Դ���Ǵ�recv��ϵͳ���ջ�����ȡ�����ݵ�ư�Ӵ�С,nLen����ÿ�ν��յ�������ʵ�ʵĴ�С  Ȼ�����ǰ�ÿ�ν��յ���ʵ������nLen�ŵ����Ƕ���ĵڶ�������_szMsgBuf���棩     ����˵_szRecv�����д�ϵͳ�������߳���������  ����û���������� ����ȡnLen ������ȵ����� Ȼ���Ƶ�_szMsgBuf ����ڶ�����������
		_lastPos += nLen;								//��¼��һ����Ϣ�ķ��˶೤ ��һ������������ٷ�  ��ֹ��Ϣ��������
		while(_lastPos>=sizeof(DataHeader))				//�����Ϣ�������������Ѿ�����һ����ϢͷDataHeader����,��ʱ�Ϳ���֪����ǰ��Ϣ��ĳ���  ��Ϊ���ܵ����������Ѿ�������һ��������Ϣͷ����������
		{

			DataHeader* header = (DataHeader*)_szMsgBuf;	//�ѵ�ǰ����Ϣ����ǿת��DataHeader����
			
			if (_lastPos>=header->dataLength)			//��ǰ��Ϣ�����������ݳ��ȴ�������Ϣ����
			{
				int nSize = _lastPos - header->dataLength;//����һ��ʣ��δ������Ϣ�����������ݳ���    ����ǰ�ȴ洢һ�����ֵ ��Ϊ��������dataLengthֵ�Ѿ������˸ı��� 

				OnNetMsg(header);						//ʹ�����Ƿ�װ�Ľӿڴ���һ���������
				//��ʣ���δ���������ǰ��
				memcpy(_szMsgBuf, _szMsgBuf+ header->dataLength, _lastPos- header->dataLength);	//�����������Ҫ�Ѵ�����������ó���  û�д����������ǰ��,����������������  ������1����Ҫ�������ݵĻ�����_szMsgBuf  ,����2��ʾ����Դ��_szMsgBuf�� header->dataLength��λ�ÿ�ʼ,���֮ǰȡ�����������ǲ���Ҫ��, ����3_lastPos- header->dataLength��_lastPos��Ŀǰ_szMsgBuf����洢�����ݵĳ��� ������һ���Ѿ�������һ����,������������ʵ�ʻ�������Ӧ�ô洢�ĳ����ǣ��ܵģ�-���Ѿ�������ģ�= _lastPos- header->dataLength  �ĳ���  ������ȣ�
				_lastPos = nSize;						//д��_lastPos =_lastPos - header->dataLength   �϶����Ե� ��Ϊ��һ��������� 
				
			}
			else
			{
				//��Ϣ����������ϢĿǰʣ�����Ϣ����һ����������Ϣ  �˳��ô�whileѭ��
				break;
			}
		}
		
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
			//	printf("<socket=%d>�յ��������Ϣ��CMD_NEW_USER_JOIN,���ݳ��ȣ�%d\n", _sock, userJoin->dataLength);

		}
		break;
		case CMD_ERROR:
		{
			///NewUserJoin* userJoin = (NewUserJoin*)header;        //Logout���ݽṹ���� ���ǽ��յ�������
			 //	printf("�յ������<_Sock%d>����CMD_NEW_USER_JOIN  ���ݳ���:%d �¼���Ŀͻ���socket:%d\n", _sock, newUserJoin->dataLength, newUserJoin->socket);
			printf("<socket=%d>�յ��������Ϣ��CMD_ERROR,���ݳ��ȣ�%d\n", _sock, header->dataLength);
		}
		break;
		default:
			printf("<socket=%d>�յ�δ֪����,���ݳ��ȣ�%d\n", _sock, header->dataLength);

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