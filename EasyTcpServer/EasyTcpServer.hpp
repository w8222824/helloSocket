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


#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240			// ʹ�ú궨��һ���ַ�����������С��Ԫ��С  ����RECV_BUFF_SIZE��ʾ����10240  10Kb��СӦ��
#endif


class ClientSocket 
{
public:
	ClientSocket(SOCKET sockfd=INVALID_SOCKET)
	{
		_sockfd = sockfd;
		memset(_szMsgBuf, 0, sizeof(_szMsgBuf));
		_lastPos = 0;
	}
	SOCKET sockfd() {
		return _sockfd;
	}
	//����Դ  Ҳ���Ǵ�ϵͳ������������ˮ��ˮư
	char* msgBuf() {
		return _szMsgBuf;
	}

	int getLastPos() {

		return _lastPos;
	}


	void setLastPos( int lastpos) {
		_lastPos = lastpos;
	}
private:
	SOCKET _sockfd;		//fd_set    file desc  set
	char _szMsgBuf[RECV_BUFF_SIZE*10] ;		//����һ���ڶ�������  ��Ϣ������
	int _lastPos;
};//class ClientSocket 

class EasyTcpServer
{
private:
	SOCKET _sock;
	std::vector<ClientSocket*> _clients;        //����һ��ȫ�������������洢�ͻ��˵�socket  ����Ϊɶ��*  ����Ϊ��д*��������ջ�ϴ�����  ����*  ����ʹ�ö�����new�����ɵ�  �������ڶ��ϴ�����

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
		SOCKET cSock = INVALID_SOCKET;        //����һ����Ч��socket��ַ
#ifdef _WIN32
		cSock = accept((int)_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
		cSock = accept((int)_sock, (sockaddr*)&clientAddr, (socklen_t *)&nAddrLen);
#endif
		if (INVALID_SOCKET == cSock)
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
			//SendDataToAll(&newUserJoin);
			_clients.push_back(new ClientSocket(cSock));							//���¼���Ŀͻ��˴洢��ȫ����������
			printf("�¿ͻ��˼���:socket=%d, IP=%s \n", (int)cSock, (inet_ntoa)(clientAddr.sin_addr));//(inet_ntoa)��ʾ��IP��ַת���ɿɶ��Ե��ַ�����
		}


		return cSock;
			
	}

	//�ر�Socket
	void Close() {

		if (INVALID_SOCKET != _sock)		//������ӷ�������ʱ��socket����Ч�� ���ǳ�ʼ��socketһ��
		{

#ifdef _WIN32
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				closesocket(_clients[n]->sockfd());
				delete _clients[n];	//new�˶��� ������Ҫɾ����

			}
			//8.�ر��׽���socket  ������Ҫ�ر��ĸ�socket  �������Լ���
			closesocket(_sock);
			//���windows socket ����
			WSACleanup();
#else
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				close(_clients[n]->sockfd());
				delete _clients[n];	//new�˶��� ������Ҫɾ����

			}
			//8.�ر��׽���socket  ������Ҫ�ر��ĸ�socket  �������Լ���
			close(_sock->sockfd());

#endif
			_clients.clear();		//���������洢�ͻ����׽���socket������
		}

	}// Close()

	
	int _nCount = 0;
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
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				FD_SET(_clients[n]->sockfd(), &fdRead);
				if (maxSock<_clients[n]->sockfd()) {
					maxSock = _clients[n]->sockfd();
				}
			}


			timeval t = { 1,0 };        //select����5������timeval  ����һ��ʱ����� �и���Ա���� һ������  һ����΢��  ����windos�ļ�ʱ�����񻹲��ܾ�ȷ��΢�� ������������� {0,0,}��ʾ������  ��ͣ��ִ��
										//nfds  ��һ������ֵ  ��ָfd_set���������е�������(socket)�ķ�Χ,����������
										//����1���������ļ����������ֵ+1  ��windows�������������ν ����д0
			int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp,/* NULL*/&t);        //���selectһ����˵���һ��������дNUll��ʾ������ģʽ ʲô������ģʽ�־��ǵ��пͻ������ӽ������Ż��з���ֵ  ��Ȼ����һֱ���������� ��������ֵ  ����һ�¾�֪��  ֱ������  �ͻ��˲����� ����һֱͣ�����ﲻ������  �����Ϊnull���ǷǶ���ģʽ��  ���ȥ�ͱ�ʾ�೤ʱ��ȥ��ѯһ��
			//printf("select ret=%d count=%d\n", ret, _nCount++);
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


			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				if (FD_ISSET(_clients[n]->sockfd(), &fdRead))
				{
					if (-1 == RecvData(_clients[n]))	//����пͻ����˳�
					{
					
						/*auto*/std::vector<ClientSocket*>::iterator iter = _clients.begin() + n;	//+n����Ҫ�Ƴ��ĵ�����
						if (iter != _clients.end())
						{
							delete _clients[n];			//ɾ��new����������
							_clients.erase(iter);

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
	char _szRecv[RECV_BUFF_SIZE] = {};        //����һ�������� ����������  recv���յ��������߷ŵ��������������

	int RecvData(ClientSocket* pClient) {
		 //5.���տͻ��˵�����
		int nLen = (int)recv(pClient->sockfd(), _szRecv, /*sizeof(DataHeader)*/409600, 0);    //���տͻ��˷��͹���������  ����1��Ӧ�ͻ��˵�socket  ����2 ���տͻ��˷������ݵĻ�����  ����3 ��������С  ����4  0Ĭ��
		//	printf("nLen=%d\n", nLen);																																  //DataHeader* header = (DataHeader*)szRecv;        //��ÿ�ν��յ������ݸ����header
		if (nLen <= 0)
		{
			printf("�ͻ���<_clientSock%d>�Ѿ��˳�,���������\n", pClient->sockfd());
			return -1;
		}
		//����ȡ�������ݿ�������Ϣ������
		memcpy(pClient->msgBuf() + pClient->getLastPos(), _szRecv, nLen);				//��ÿ�ν������ݷŵ����Ƕ���ĵڶ�������_szMsgBuf���棨����_szRecv���Դ���Ǵ�recv��ϵͳ���ջ�����ȡ�����ݵ�ư�Ӵ�С,nLen����ÿ�ν��յ�������ʵ�ʵĴ�С  Ȼ�����ǰ�ÿ�ν��յ���ʵ������nLen�ŵ����Ƕ���ĵڶ�������_szMsgBuf���棩     ����˵_szRecv�����д�ϵͳ�������߳���������  ����û���������� ����ȡnLen ������ȵ����� Ȼ���Ƶ�_szMsgBuf ����ڶ�����������
	
		pClient->setLastPos(pClient->getLastPos() + nLen);							//��¼��һ����Ϣ�ķ��˶೤ ��һ������������ٷ�  ��ֹ��Ϣ��������
		while (pClient->getLastPos() >= sizeof(DataHeader))				//�����Ϣ�������������Ѿ�����һ����ϢͷDataHeader����,��ʱ�Ϳ���֪����ǰ��Ϣ��ĳ���  ��Ϊ���ܵ����������Ѿ�������һ��������Ϣͷ����������
		{

			DataHeader* header = (DataHeader*)pClient->msgBuf();	//�ѵ�ǰ����Ϣ����ǿת��DataHeader����

			if (pClient->getLastPos() >= header->dataLength)			//��ǰ��Ϣ�����������ݳ��ȴ�������Ϣ����
			{
				int nSize = pClient->getLastPos() - header->dataLength;//����һ��ʣ��δ������Ϣ�����������ݳ���    ����ǰ�ȴ洢һ�����ֵ ��Ϊ��������dataLengthֵ�Ѿ������˸ı��� 

				OnNetMsg(pClient->sockfd(),header);						//ʹ�����Ƿ�װ�Ľӿڴ���һ���������
														//��ʣ���δ���������ǰ��
				memcpy(/*_szMsgBuf*/pClient->msgBuf(), /*_szMsgBuf*/pClient->msgBuf() + header->dataLength,/* _lastPos*/pClient->getLastPos() - header->dataLength);	//�����������Ҫ�Ѵ�����������ó���  û�д����������ǰ��,����������������  ������1����Ҫ�������ݵĻ�����_szMsgBuf  ,����2��ʾ����Դ��_szMsgBuf�� header->dataLength��λ�ÿ�ʼ,���֮ǰȡ�����������ǲ���Ҫ��, ����3_lastPos- header->dataLength��_lastPos��Ŀǰ_szMsgBuf����洢�����ݵĳ��� ������һ���Ѿ�������һ����,������������ʵ�ʻ�������Ӧ�ô洢�ĳ����ǣ��ܵģ�-���Ѿ�������ģ�= _lastPos- header->dataLength  �ĳ���  ������ȣ�
				pClient->setLastPos(nSize);		/*_lastPos = nSize;*/						//д��_lastPos =_lastPos - header->dataLength   �϶����Ե� ��Ϊ��һ��������� 

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
	void OnNetMsg(SOCKET cSock,DataHeader* header) {
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			//����2����ƫ��sizeof(DataHeader)�ĳ���   ������һ������dataLength����-DataHeader�ĳ���
			Login*  login = (Login*)header;        //Login���ݽṹ���� ���ǽ��յ�������
			//printf("�յ��ͻ���<_clientSock%d>����CMD_LOGIN�����ݳ���:%d, userName=%s PassWord=%s \n", cSock, login->dataLength, login->userName, login->PassWord);
			//�����ж��û������Ƿ���ȷ����
			LoginResult  loginRet;        //��Ϊ�Ǳ����� ���Բ�����ô�����ֵ��
			//send(cSock, (char*)&loginRet, sizeof(LoginResult), 0);    //����Ӧ�ͻ��˷��Ͷ�Ӧ�ĵ�¼�����Ϣ����
			//SendData(cSock, &loginRet);
		}
		break;
		case CMD_LOGOUT:
		{
			Logout*  logout = (Logout*)header;        //Logout���ݽṹ���� ���ǽ��յ�������
			//printf("�յ��յ��ͻ���<_clientSock%d>����CMD_LOGOUT  ���ݳ���:%d userName=%s \n", cSock, logout->dataLength, logout->userName);
			//�����ж��û������Ƿ���ȷ����
			LogoutResult  logoutRet;        //��Ϊ�Ǳ����� ���Բ�����ô�����ֵ��
			//send(cSock, (char*)&logoutRet, sizeof(LogoutResult), 0);    //����Ӧ�ͻ��˷��Ͷ�Ӧ�ĵ�¼�����Ϣ����
		//	SendData(cSock, &logoutRet);
		}
		break;
		default:
		{
			printf("�յ�δ���������������ݳ���:%d  \n", cSock, header->dataLength);

			DataHeader header; //�ߵ���һ���϶�����2�������� ֱ�Ӹ��Ҳ�����ö�� ERROR  �ڷ��ظ��ͻ���
			//send(cSock, (char*)&header, sizeof(DataHeader), 0);    //����Ӧ�ͻ��˷��Ͷ�Ӧ�ĵ�¼�����Ϣ����
			SendData(cSock, &header);
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
			for (int n = (int)_clients.size() - 1; n >= 0; n--)    //���ﴦ����¿ͻ��˼����֪ͨ���͸������Ѿ�����Ŀͻ���
			{
				//send(g_clients[n], (const char*)header, header->dataLength, 0);        //���͸���ǰ�������Ѿ�֪�������ϵĿͻ���
				SendData(_clients[n]->sockfd(), header);
			}
		}	
		
	}//SendDataToAll()

};

#endif  _EasyTcpServer_hpp_