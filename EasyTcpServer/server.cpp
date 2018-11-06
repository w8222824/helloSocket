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

enum   CMD
{
	CMD_LOGIN,                //��¼
	CMD_LOGIN_RESULT,        //��¼����
	CMD_LOGOUT,                //�ǳ�
	CMD_LOGOUT_RESULT,        //�ǳ�����
	CMD_NEW_USER_JOIN,        //���û�����
	CMD_ERROR                //��������

};

//��ͷ
struct DataHeader
{
	short dataLength;        //���ݳ���
	short cmd;                //����


};

struct Login :public DataHeader        //д�ɼ̳�  Ȼ���ڽṹ��Ĺ��캯������д����ͷ������   �Ͳ��÷ֱ��Ͱ�ͷ�Ͱ����� ����һ������
{
	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];

};
//DataPackage  ���շ��������ص����ݵİ���
struct LoginResult :public DataHeader
{
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};
//�¿ͻ��˵����ݽṹ
struct NewUserJoin :public DataHeader
{
	NewUserJoin() {
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		socket = 0;
	}
	int socket;
};



//DataPackage  �ǳ����ݵİ���
struct Logout :public DataHeader
{
	Logout() {
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};
//DataPackage  ���������յ��ǳ��󷵻ص����ݵİ���
struct LogoutResult :public DataHeader
{
	LogoutResult() {
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};



std::vector<SOCKET> g_clients;        //����һ��ȫ�������������洢�ͻ��˵�socket

									  //����һ���������Ѵ���ͻ��˵��߼����Ž���
int processor(SOCKET _clientSock) {

	char szRecv[4096] = {};        //����һ�������� ����������  recv���յ��������߷ŵ��������������
								   //5.���տͻ��˵�����
	int nLen = (int)recv(_clientSock, szRecv, sizeof(DataHeader), 0);    //���տͻ��˷��͹���������  ����1��Ӧ�ͻ��˵�socket  ����2 ���տͻ��˷������ݵĻ�����  ����3 ��������С  ����4  0Ĭ��
	DataHeader* header = (DataHeader*)szRecv;        //��ÿ�ν��յ������ݸ����header
	if (nLen <= 0)
	{
		printf("�ͻ���<_clientSock%d>�Ѿ��˳�,���������\n", _clientSock);
		return -1;
	}
	//if (nLen >= header->dataLength)        //�����������жϲŴ���������һ�����ݻ�������  �������ٰ�
	//{

	//}


	switch (header->cmd)
	{


	case CMD_LOGIN:
	{

		//����2����ƫ��sizeof(DataHeader)�ĳ���   ������һ������dataLength����-DataHeader�ĳ���
		recv(_clientSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);    //����2���ݵ�������С ��Ϊ��������Login�Ѿ���DataHeader���̳��� ���ݶ���һ�����շ�  ���Ի����СҪ������ӣ�ָ���ƫ�ƣ�   ����3���� ��Ϊ���������Ѿ�ȡ������Ϣͷ Ҫ��ȡ��λ��ҲҪ����DataHeader���λ��   2-14  06:44  ���������ָ�������
		Login*  login = (Login*)szRecv;        //Login���ݽṹ���� ���ǽ��յ�������
		printf("�յ��ͻ���<_clientSock%d>����CMD_LOGIN�����ݳ���:%d, userName=%s PassWord=%s \n", _clientSock, login->dataLength, login->userName, login->PassWord);
		//�����ж��û������Ƿ���ȷ����
		LoginResult  loginRet;        //��Ϊ�Ǳ����� ���Բ�����ô�����ֵ��
		send(_clientSock, (char*)&loginRet, sizeof(LoginResult), 0);    //����Ӧ�ͻ��˷��Ͷ�Ӧ�ĵ�¼�����Ϣ����
	}
	break;
	case CMD_LOGOUT:
	{

		recv(_clientSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);    //���հ�ͷ��CMD_LOGOUT������
		Logout*  logout = (Logout*)szRecv;        //Logout���ݽṹ���� ���ǽ��յ�������
		printf("�յ��յ��ͻ���<_clientSock%d>����CMD_LOGOUT  ���ݳ���:%d userName=%s \n", _clientSock, logout->dataLength, logout->userName);
		//�����ж��û������Ƿ���ȷ����

		LogoutResult  logoutRet;        //��Ϊ�Ǳ����� ���Բ�����ô�����ֵ��
		send(_clientSock, (char*)&logoutRet, sizeof(LogoutResult), 0);    //����Ӧ�ͻ��˷��Ͷ�Ӧ�ĵ�¼�����Ϣ����

	}
	break;
	default:
	{
		DataHeader header = { 0, CMD_ERROR };//�ߵ���һ���϶�����2�������� ֱ�Ӹ��Ҳ�����ö�� ERROR  �ڷ��ظ��ͻ���
		send(_clientSock, (char*)&header, sizeof(DataHeader), 0);    //����Ӧ�ͻ��˷��Ͷ�Ӧ�ĵ�¼�����Ϣ����

	}

	break;
	}
	return 0;
}
int main() {
#ifdef _WIN32
	//����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif

	///
	//--��Socket API��������TCP�����
	// 1.����һ��socket �׽���    ��ô�����׽���socket����  ����1��ʾipv4���͵�(ipv6��AF_INET6)  ������ʲô�������͵�sock  �����������͵�   ������������Ҫ����ʲô���͵�socket ������tcp���͵�  ��������һ���׽���
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 2.���ӷ�����connect  ����1���׽���socket  ����2��sockaddr���͵ĵ�ַ   ����3�ǵ�ַ�ĳ���
	sockaddr_in _sin = {};        //��ʼ��sockaddr Ϊʲôд��sockaddr_in  �����Ǻ���������Ҫ��sockaddr  ����Ϊsockaddr_in��sockaddr_in��һ�����͵�  ֻ��sockaddr_in�����Ը�������д���ǵĴ������ת�������Լ�ȥ����2������������
	_sin.sin_family = AF_INET;    //Э��� һ����AF_INET��ʾTCP/IPЭ�顣  �����ʾipv4
	_sin.sin_port = htons(4567)/*4567*/;//Ϊʲô����4567����������  ��Ϊ����Ҫת�� host to net unsigned short����������������ת����������޷��ŵĶ��ַ����������ͣ�  �����ʾ���ӵ�����˿ںŵļ���

#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");//sin_addr��һ�������壬��������Ϳ���ʹ�ö��ַ�ʽ��ʾIP��ַ��һ������ʹ�����е����һ�ַ�ʽ�������޷��ų�������������ʾIP��ַ����ʱ���õ���s_nu���ݳ�Ա��s_un��һ���ṹ�壬����ֻ��һ�����ݳ�Ա����s_addr��           inet_addr("127.0.0.1");    //"127.0.0.1"�����ĵ�ַ ��ôд���ܷ�ֹ�����ķ���,  INADDR_ANY��ʾ������ȫ��ip��ַ�����Է���
#else
	_sin.sin_addr.s_addr = INADDR_ANY;// inet_addr("192.168.236.1");
#endif
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))//bind �����ڽ��ܿͻ������ӵ�����˿�
	{
		//������ʾbind��ʧ��
		printf("ERROR,�����ڽ��ܿͻ������ӵ�����˿�ʧ�ܣ����ܱ�ռ�á�\n");
	}
	else
	{
		printf("������˿ڳɹ���\n");

	}
	// 3.listen ��������˿�   ����1��ʾsock  ����2��ʾ�󶨵�ʱ�������ܶ�����ͬʱ����
	if (SOCKET_ERROR == listen(_sock, 5)) {

		printf("��������˿�ʧ�ܡ�\n");
	}
	else
	{
		printf("��������˿ڳɹ���\n");

	}


	//char _recvBuf[128] = {};
	while (true)        //��while��true���������Ϳ��Խ��ն����Ϣ��
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
			break;
		}

		if (FD_ISSET(_sock, &fdRead))    //�ж�������(Socket)�Ƿ��ڼ�����
		{
			FD_CLR(_sock, &fdRead);        //����һ��  �������ܼ���ֻ�������˼����� û�������Ӧ������
										   // 4.accept �ȴ����ܿͻ�������  ����1���Ƿ���˵�socket  ����2�ǽ��յĿͻ��˵�socket  ����3��ʾ�ͻ���socket�ĳ���
			sockaddr_in clientAddr = {};    //��ʼ���ͻ��˵�socket��ַ
			int nAddrLen = sizeof(sockaddr_in);    //��ʼ��һ��Int���͵� socket�ĳ���  ������������տͻ��˵�socket�ĵ�ַ���ȵ�
			SOCKET _clientSock = INVALID_SOCKET;        //����һ����Ч��socket��ַ
#ifdef _WIN32
			_clientSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
			_clientSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t *)&nAddrLen);
#endif
			if (INVALID_SOCKET == _clientSock)
			{
				//������ʾ���յ���socket����������Ч��
				printf("����,���յ���Ч�ͻ���socket��\n");
			}
			else
			{

				for (int n = (int)g_clients.size() - 1; n >= 0; n--)    //���ﴦ����¿ͻ��˼����֪ͨ���͸������Ѿ�����Ŀͻ���
				{
					NewUserJoin newUserJoin;                            //���������洢�����¿ͻ��˼���֪ͨ���ݵ����ݽṹ
					send(g_clients[n], (const char*)&newUserJoin, sizeof(NewUserJoin), 0);        //���͸���ǰ�������Ѿ�֪�������ϵĿͻ���
				}
				g_clients.push_back(_clientSock);        //���¼���Ŀͻ��˴洢��ȫ����������
				printf("�¿ͻ��˼���:socket=%d, IP=%s \n", (int)_clientSock, (inet_ntoa)(clientAddr.sin_addr));//(inet_ntoa)��ʾ��IP��ַת���ɿɶ��Ե��ַ�����
			}

		}


		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			if (FD_ISSET(g_clients[n], &fdRead))
			{
				if (-1 == processor(g_clients[n]))	//����пͻ����˳�
				{
					/*auto*/std::vector<SOCKET>::iterator iter = g_clients.begin() + n;	//+n����Ҫ�Ƴ��ĵ�����
					if (iter != g_clients.end())
					{
						g_clients.erase(iter);

					}
				}
			}
		}

		// printf("����˿���ʱ�䴦������ҵ�񡣡�selectΪ�Ƕ���ģʽ�ô�ӡ�᲻��ִ��\n");

	}
	//ִ�е��������ʾ�����ret<0 ���������  �����е�socket���رյ�
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
	printf("���˳����������\n");
	getchar();

	return 0;
}
