//���⻥������������  windows������һЩsocke����������  û������ܻᵼ�º궨���ͻ
#define WIN32_LEAN_AND_MEAN	
//Ϊ��ʹ�ù�ʱ��inet_ntoa  Ҫôʹ�������  Ҫô��������������������C/C++�����SDK�����ر���
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>	//printf��Ҫ�����ͷ�ļ�
//#pragma comment(lib,"ws2_32.lib")		// û����仰 ִ�е�WSAStartup(ver, &dat);�ᱨ�޷����� �޷��������ⲿ���� __imp__WSAStartup@8�Ĵ���   ��仰����˼������Ҫһ����̬���ӿ�  ���ֻ����windows�²����� ��������Ҫ��ƽ̨ ���������Ҽ���Ŀ���Ե�������� �ڵ������ �ڸ����������������ws2_32.lib

int main() {

	WORD ver = MAKEWORD(2,2);
	WSADATA dat;

	WSAStartup(ver, &dat);

	///
	//--��Socket API��������TCP�ͻ���
	// 1.����һ��socket �׽���    ��ô�����׽���socket����  ����1��ʾipv4���͵�(ipv6��AF_INET6)  ������ʲô�������͵�sock  �����������͵�   ������������Ҫ����ʲô���͵�socket ������tcp���͵�  ��������һ���׽���
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 2.���ӷ�����connect  ����1���׽���socket  ����2��sockaddr���͵ĵ�ַ   ����3�ǵ�ַ�ĳ���
	sockaddr_in _sin = {};		//��ʼ��sockaddr Ϊʲôд��sockaddr_in  �����Ǻ���������Ҫ��sockaddr  ����Ϊsockaddr_in��sockaddr_in��һ�����͵�  ֻ��sockaddr_in�����Ը�������д���ǵĴ������ת�������Լ�ȥ����2������������
	_sin.sin_family = AF_INET;	//Э��� һ����AF_INET��ʾTCP/IPЭ�顣  �����ʾipv4
	_sin.sin_port = htons(4567)/*4567*/;//Ϊʲô����4567����������  ��Ϊ����Ҫת�� host to net unsigned short����������������ת����������޷��ŵĶ��ַ����������ͣ�
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//sin_addr��һ�������壬��������Ϳ���ʹ�ö��ַ�ʽ��ʾIP��ַ��һ������ʹ�����е����һ�ַ�ʽ�������޷��ų�������������ʾIP��ַ����ʱ���õ���s_nu���ݳ�Ա��s_un��һ���ṹ�壬����ֻ��һ�����ݳ�Ա����s_addr��           inet_addr("127.0.0.1");	//"127.0.0.1"�����ĵ�ַ ��ôд���ܷ�ֹ�����ķ���,  INADDR_ANY��ʾ������ȫ��ip��ַ�����Է���
	if (SOCKET_ERROR==bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))//bind �����ڽ��ܿͻ������ӵ�����˿�
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

	// 4.accept �ȴ����ܿͻ�������  ����1���Ƿ���˵�socket  ����2�ǽ��յĿͻ��˵�socket  ����3��ʾ�ͻ���socket�ĳ���
	sockaddr_in clientAddr = {};	//��ʼ���ͻ��˵�socket��ַ
	int nAddrLen = sizeof(sockaddr_in);	//��ʼ��һ��Int���͵� socket�ĳ���  ������������տͻ��˵�socket�ĵ�ַ���ȵ�
	SOCKET _clientSock = INVALID_SOCKET;		//����һ����Ч��socket��ַ



	char msgBuf[] = "Hello, I,m  server";
	while (true)		//��while��true���������Ϳ��Խ��ն����Ϣ��
	{
		_clientSock = accept(_sock, (sockaddr*)&clientAddr,/* (int*)sizeof(clientAddr)*/&nAddrLen);
		if (INVALID_SOCKET == _clientSock)
		{
			//������ʾ���յ���socket����������Ч��
			printf("����,���յ���Ч�ͻ���socket��\n");
		}


		printf("�¿ͻ��˼���:IP=%s \n", (inet_ntoa)(clientAddr.sin_addr));//(inet_ntoa)��ʾ��IP��ַת���ɿɶ��Ե��ַ�����
		//5 send ��ͻ��˷���һ������
	//	char msgBuf[] = "Hello, I,m  server";  ��while��������  ���д��while����
		//����1��Ҫ������Ϣ�Ŀͻ��ٵ�socket ����2��Ҫ���Ͷ�Ӧsocket���ַ�������  ����3�����ݵĳ���   +1��ʾ�ѽ�β��һ�����͹�ȥ    ����4 0��ʾĬ�� ��֪������
		send(_clientSock, msgBuf, strlen(msgBuf) + 1, 0);
	}
	

		//6.�ر��׽���socket  ������Ҫ�ر��ĸ�socket  �������Լ���
		closesocket(_sock);



	//--��Socket API��������TCP�����
	// 1.����һ��socket
	// 2.bind ���û����ܿͻ������ӵ�����˿�
	// 3.listen ��������˿�
	// 4.accept �ȴ����ܿͻ�������
	// 5.send ��ͻ��˷���һ������
	// 6.�ر��׽���closesocket
	//--------------
	//���windows socket ����


	///

	WSACleanup();


	return 0;
}