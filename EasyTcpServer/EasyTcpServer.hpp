#ifndef _EasyTcpServer_hpp_					//���û�ж���������δ��� 

#define  _EasyTcpServer_hpp_				//������  



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
#include "MessageHeader.hpp"
#pragma comment(lib,"ws2_32.lib")        // û����仰 ִ�е�WSAStartup(ver, &dat);�ᱨ�޷����� �޷��������ⲿ���� __imp__WSAStartup@8�Ĵ���   ��仰����˼������Ҫһ����̬���ӿ�  ���ֻ����windows�²����� ��������Ҫ��ƽ̨ ���������Ҽ���Ŀ���Ե�������� �ڵ������ �ڸ����������������ws2_32.lib






class EasyTcpServer
{
private:
	SOCKET _sock;
	std::vector<SOCKET> g_clients;        //����һ��ȫ�������������洢�ͻ��˵�socket

public:	
	EasyTcpServer() {
		_sock = INVALID_SOCKET;//����һ����Ч��socket  
	}

	virtual ~EasyTcpServer() {
		Close();
	}

	//��ʼ��Socket
	int InitSocket() {
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
			printf("<socket=%d>��ʼ������socket����Ч�ģ���ʼ�رվ����ӡ�\n", (int)_sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//��������   IPv4  ��������  Э�鴫�����tcp
		if (INVALID_SOCKET == _sock)        //�����׽���socketʧ��
		{
			printf("���󣬽���Socket=<%d>ʧ�ܡ�\n", (int)_sock);
		}
		else
		{
			printf("��ȷ������Socket=<%d>�ɹ���\n", (int)_sock);
		}

		return  _sock;
	}//InitSocket()


	//��IP�Ͷ˿ں�
	int Bind(const char* ip ,unsigned short port) {
		//�����ʼ��socketû�гɹ�
		if (INVALID_SOCKET==_sock)
		{
			InitSocket();	//�ڳ�ʼ��һ��
		}

		// 2.���ӷ�����connect  ����1���׽���socket  ����2��sockaddr���͵ĵ�ַ   ����3�ǵ�ַ�ĳ���
		sockaddr_in _sin = {};        //��ʼ��sockaddr Ϊʲôд��sockaddr_in  �����Ǻ���������Ҫ��sockaddr  ����Ϊsockaddr_in��sockaddr_in��һ�����͵�  ֻ��sockaddr_in�����Ը�������д���ǵĴ������ת�������Լ�ȥ����2������������
		_sin.sin_family = AF_INET;    //Э��� һ����AF_INET��ʾTCP/IPЭ�顣  �����ʾipv4
		_sin.sin_port = htons(port)/*4567*/;//Ϊʲô����4567����������  ��Ϊ����Ҫת�� host to net unsigned short����������������ת����������޷��ŵĶ��ַ����������ͣ�  �����ʾ���ӵ�����˿ںŵļ���

#ifdef _WIN32
		if (ip)					//���ھͰ����ip  
		{
			_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else
		{
			_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");//sin_addr��һ�������壬��������Ϳ���ʹ�ö��ַ�ʽ��ʾIP��ַ��һ������ʹ�����е����һ�ַ�ʽ�������޷��ų�������������ʾIP��ַ����ʱ���õ���s_nu���ݳ�Ա��s_un��һ���ṹ�壬����ֻ��һ�����ݳ�Ա����s_addr��           inet_addr("127.0.0.1");    //"127.0.0.1"�����ĵ�ַ ��ôд���ܷ�ֹ�����ķ���,  INADDR_ANY��ʾ������ȫ��ip��ַ�����Է���
		}
	
#else
		if (ip)					//���ھͰ����ip  
		{
			_sin.sin_addr.s_addr = inet_addr(ip);
		}
		else
		{
			_sin.sin_addr.s_addr = INADDR_ANY;// inet_addr("192.168.236.1");	
		}
#endif
		int ret = bind(_sock, (sockaddr*)&_sin, sizeof(_sin));//bind �����ڽ��ܿͻ������ӵ�����˿�

		if (SOCKET_ERROR == ret)//bind �����ڽ��ܿͻ������ӵ�����˿�
		{
			//������ʾbind��ʧ��
			printf("ERROR,�����ڽ��ܿͻ������ӵ�����˿�<%d>ʧ�ܣ����ܱ�ռ�á�\n",port);
		}
		else
		{
			printf("������˿�<%d>�ɹ���\n", port);

		}

		return 1;	//1��������������true����˼
	}//Bind()

	//�����˿ں�  ����n��ʾ���������֧��n������
	int Listen( int n) 
	{

		// 3.listen ��������˿�   ����1��ʾsock  ����2��ʾ�󶨵�ʱ�������ܶ�����ͬʱ����
		int ret = listen(_sock, n);
		if (SOCKET_ERROR == ret) {

			printf("Socket=<%d>��������˿�ʧ�ܡ�\n", (int)_sock);
		}
		else
		{
			printf("Socket=<%d>��������˿ڳɹ���\n", (int)_sock);

		}
		return ret;
	}//Listen()

	//���տͻ�������
	SOCKET Accept() {
		sockaddr_in clientAddr = {};    //��ʼ���ͻ��˵�socket��ַ
		int nAddrLen = sizeof(sockaddr_in);    //��ʼ��һ��Int���͵� socket�ĳ���  ������������տͻ��˵�socket�ĵ�ַ���ȵ�
		SOCKET _clientSock = INVALID_SOCKET;        //����һ����Ч��socket��ַ
#ifdef _WIN32
		_clientSock = accept((int)_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
		_clientSock = accept((int)_sock, (sockaddr*)&clientAddr, (socklen_t *)&nAddrLen);
#endif
		if (INVALID_SOCKET == _clientSock)
		{
			//������ʾ���յ���socket����������Ч��
			printf("Socket=<%d>����,���յ���Ч�ͻ���socket��\n", (int)_sock);
		}
		else
		{
			//for (int n = (int)g_clients.size() - 1; n >= 0; n--)    //���ﴦ����¿ͻ��˼����֪ͨ���͸������Ѿ�����Ŀͻ���
			//{
			//	NewUserJoin newUserJoin;                            //���������洢�����¿ͻ��˼���֪ͨ���ݵ����ݽṹ
			//	send(g_clients[n], (const char*)&newUserJoin, sizeof(NewUserJoin), 0);        //���͸���ǰ�������Ѿ�֪�������ϵĿͻ���
			//}
			NewUserJoin newUserJoin;									//���������洢�����¿ͻ��˼���֪ͨ���ݵ����ݽṹ
			SendDataToAll(&newUserJoin);
			g_clients.push_back(_clientSock);							//���¼���Ŀͻ��˴洢��ȫ����������
			printf("�¿ͻ��˼���:socket=%d, IP=%s \n", (int)_clientSock, (inet_ntoa)(clientAddr.sin_addr));//(inet_ntoa)��ʾ��IP��ַת���ɿɶ��Ե��ַ�����
		}


		return _clientSock;
			
	}

	//�ر�Socket
	void Close() {

		if (INVALID_SOCKET != _sock)		//������ӷ�������ʱ��socket����Ч�� ���ǳ�ʼ��socketһ��
		{

#ifdef _WIN32
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				closesocket(g_clients[n]);

			}
			//8.�ر��׽���socket  ������Ҫ�ر��ĸ�socket  �������Լ���
			closesocket(_sock);
			//���windows socket ����
			WSACleanup();
#else
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				close(g_clients[n]);

			}
			//8.�ر��׽���socket  ������Ҫ�ر��ĸ�socket  �������Լ���
			close(_sock);
#endif
		}

	}// Close()

	//����������Ϣ
	bool OnRun() {

		if (isRun())
		{
			//������ socket ������  windows ���� ��һ������û������  ��Ȼ��д����_sock+1 ����ʵ������windows�ϲ�������
			fd_set fdRead;    //������һ��������(socket)����
			fd_set fdWrite;
			fd_set fdExp;
			//����3����������fd_set���
			FD_ZERO(&fdRead);    //������
			FD_ZERO(&fdWrite);
			FD_ZERO(&fdExp);
			//�����3���������ö�Ӧsocket��Ӧ����û�пɶ�  ��д  �쳣�Ĳ���
			FD_SET(_sock, &fdRead);    //��_sock���뼯��
			FD_SET(_sock, &fdWrite);
			FD_SET(_sock, &fdExp);

			SOCKET maxSock = _sock;
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				FD_SET(g_clients[n], &fdRead);
				if (maxSock<g_clients[n]) {
					maxSock = g_clients[n];
				}
			}


			timeval t = { 1,0 };        //select����5������timeval  ����һ��ʱ����� �и���Ա���� һ������  һ����΢��  ����windos�ļ�ʱ�����񻹲��ܾ�ȷ��΢�� ������������� {0,0,}��ʾ������  ��ͣ��ִ��
										//nfds  ��һ������ֵ  ��ָfd_set���������е�������(socket)�ķ�Χ,����������
										//����1���������ļ����������ֵ+1  ��windows�������������ν ����д0
			int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp,/* NULL*/&t);        //���selectһ����˵���һ��������дNUll��ʾ������ģʽ ʲô������ģʽ�־��ǵ��пͻ������ӽ������Ż��з���ֵ  ��Ȼ����һֱ���������� ��������ֵ  ����һ�¾�֪��  ֱ������  �ͻ��˲����� ����һֱͣ�����ﲻ������  �����Ϊnull���ǷǶ���ģʽ��  ���ȥ�ͱ�ʾ�೤ʱ��ȥ��ѯһ��
			if (ret<0)
			{
				printf("select�������\n");
			
				return false;
			}

			if (FD_ISSET(_sock, &fdRead))    //�ж�������(Socket)�Ƿ��ڼ�����
			{
				FD_CLR(_sock, &fdRead);        //����һ��  �������ܼ���ֻ�������˼����� û�������Ӧ������
				Accept();							   // 4.accept �ȴ����ܿͻ�������  ����1���Ƿ���˵�socket  ����2�ǽ��յĿͻ��˵�socket  ����3��ʾ�ͻ���socket�ĳ���


			}


			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				if (FD_ISSET(g_clients[n], &fdRead))
				{
					if (-1 == RecvData(g_clients[n]))	//����пͻ����˳�
					{
						/*auto*/std::vector<SOCKET>::iterator iter = g_clients.begin() + n;	//+n����Ҫ�Ƴ��ĵ�����
						if (iter != g_clients.end())
						{
							g_clients.erase(iter);

						}
					}
				}
			}
			return true;
		}//if (isRun())
		return false;
	}

	//�Ƿ�����
	bool  isRun() {

		return _sock != INVALID_SOCKET;
	}

	//��������  ����ճ�� ��ְ�

	int RecvData(SOCKET _cSock) {


		char szRecv[4096] = {};        //����һ�������� ����������  recv���յ��������߷ŵ��������������
									   //5.���տͻ��˵�����
		int nLen = (int)recv(_cSock, szRecv, sizeof(DataHeader), 0);    //���տͻ��˷��͹���������  ����1��Ӧ�ͻ��˵�socket  ����2 ���տͻ��˷������ݵĻ�����  ����3 ��������С  ����4  0Ĭ��
		DataHeader* header = (DataHeader*)szRecv;        //��ÿ�ν��յ������ݸ����header
		if (nLen <= 0)
		{
			printf("�ͻ���<_clientSock%d>�Ѿ��˳�,���������\n", _cSock);
			return -1;
		}
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);    //����2���ݵ�������С ��Ϊ��������Login�Ѿ���DataHeader���̳��� ���ݶ���һ�����շ�  ���Ի����СҪ������ӣ�ָ���ƫ�ƣ�   ����3���� ��Ϊ���������Ѿ�ȡ������Ϣͷ Ҫ��ȡ��λ��ҲҪ����DataHeader���λ��   2-14  06:44  ���������ָ�������

		OnNetMsg(_cSock, header);	//��������
		return 0;
	}//int processor(SOCKET _Sock)

	//��Ӧ������Ϣ
	void OnNetMsg(SOCKET _cSock,DataHeader* header) {
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			//����2����ƫ��sizeof(DataHeader)�ĳ���   ������һ������dataLength����-DataHeader�ĳ���
			Login*  login = (Login*)header;        //Login���ݽṹ���� ���ǽ��յ�������
			printf("�յ��ͻ���<_clientSock%d>����CMD_LOGIN�����ݳ���:%d, userName=%s PassWord=%s \n", _cSock, login->dataLength, login->userName, login->PassWord);
			//�����ж��û������Ƿ���ȷ����
			LoginResult  loginRet;        //��Ϊ�Ǳ����� ���Բ�����ô�����ֵ��
			send(_cSock, (char*)&loginRet, sizeof(LoginResult), 0);    //����Ӧ�ͻ��˷��Ͷ�Ӧ�ĵ�¼�����Ϣ����
		}
		break;
		case CMD_LOGOUT:
		{
			Logout*  logout = (Logout*)header;        //Logout���ݽṹ���� ���ǽ��յ�������
			printf("�յ��յ��ͻ���<_clientSock%d>����CMD_LOGOUT  ���ݳ���:%d userName=%s \n", _cSock, logout->dataLength, logout->userName);
			//�����ж��û������Ƿ���ȷ����
			LogoutResult  logoutRet;        //��Ϊ�Ǳ����� ���Բ�����ô�����ֵ��
			send(_cSock, (char*)&logoutRet, sizeof(LogoutResult), 0);    //����Ӧ�ͻ��˷��Ͷ�Ӧ�ĵ�¼�����Ϣ����
		}
		break;
		default:
		{
			DataHeader header = { 0, CMD_ERROR };//�ߵ���һ���϶�����2�������� ֱ�Ӹ��Ҳ�����ö�� ERROR  �ڷ��ظ��ͻ���
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);    //����Ӧ�ͻ��˷��Ͷ�Ӧ�ĵ�¼�����Ϣ����
		}
		break;
		}
	}//OnNetMsg

	//���͸�ָ��socket����
	int SendData(SOCKET _cSock,DataHeader* header) {

		if (isRun() && header)	//socket�Ƿ�������  header��Ϊ��
		{
			return send(_cSock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}//SendData()

	//���������ӵ�socket��������
	void SendDataToAll( DataHeader* header) 
	{

		if (isRun() && header)	//socket�Ƿ�������  header��Ϊ��
		{
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)    //���ﴦ����¿ͻ��˼����֪ͨ���͸������Ѿ�����Ŀͻ���
			{
				//send(g_clients[n], (const char*)header, header->dataLength, 0);        //���͸���ǰ�������Ѿ�֪�������ϵĿͻ���
				SendData(g_clients[n], header);
			}
		}	
		
	}//SendDataToAll()

};

#endif  _EasyTcpServer_hpp_