//避免互相依赖的引用  windows里面有一些socke的早期引用  没这个可能会导致宏定义冲突
#define WIN32_LEAN_AND_MEAN	
//为了使用过时的inet_ntoa  要么使用这个宏  要么在属性里面的配置里面的C/C++里面把SDK检查给关闭了
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>	//printf需要导入的头文件
//#pragma comment(lib,"ws2_32.lib")		// 没有这句话 执行到WSAStartup(ver, &dat);会报无法解析 无法解析的外部符号 __imp__WSAStartup@8的错误   这句话的意思是我需要一个静态链接库  这个只能在windows下才有用 但是我们要跨平台 所以我们右键项目属性点击连接器 在点击输入 在附加依赖项里面添加ws2_32.lib

int main() {

	WORD ver = MAKEWORD(2,2);
	WSADATA dat;

	WSAStartup(ver, &dat);

	///
	//--用Socket API建立简易TCP客户端
	// 1.建立一个socket 套接字    怎么建立套接字socket函数  参数1表示ipv4类型的(ipv6是AF_INET6)  参数是什么数据类型的sock  这里是流类型的   参数三是我们要创建什么类型的socket 这里是tcp类型的  函数返回一个套接字
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 2.连接服务器connect  参数1是套接字socket  参数2是sockaddr类型的地址   参数3是地址的长度
	sockaddr_in _sin = {};		//初始化sockaddr 为什么写成sockaddr_in  而不是函数里面需要的sockaddr  是因为sockaddr_in和sockaddr_in是一个类型的  只是sockaddr_in的属性更方便填写我们的代码可以转到定义自己去看下2个的属性区别
	_sin.sin_family = AF_INET;	//协议簇 一般用AF_INET表示TCP/IP协议。  这里表示ipv4
	_sin.sin_port = htons(4567)/*4567*/;//为什么不用4567这样的数字  因为我们要转换 host to net unsigned short（主机的数据类型转换成网络的无符号的短字符的数据类型）
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//sin_addr是一个联合体，用联合体就可以使用多种方式表示IP地址。一般我们使用其中的最后一种方式，即用无符号长整型数据来表示IP地址。此时，用的是s_nu数据成员，s_un是一个结构体，其中只有一个数据成员，即s_addr。           inet_addr("127.0.0.1");	//"127.0.0.1"本机的地址 这么写还能防止外网的访问,  INADDR_ANY表示本机的全部ip地址都可以访问
	if (SOCKET_ERROR==bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))//bind 绑定用于接受客户端连接的网络端口
	{
		//进来表示bind绑定失败
		printf("ERROR,绑定用于接受客户端连接的网络端口失败，可能被占用。\n");
	}
	else
	{
		printf("绑定网络端口成功。\n");

	}
	// 3.listen 监听网络端口   参数1表示sock  参数2表示绑定的时候最大接受多少人同时连接
	if (SOCKET_ERROR == listen(_sock, 5)) {

		printf("监听网络端口失败。\n");
	}
	else
	{
		printf("监听网络端口成功。\n");

	}

	// 4.accept 等待接受客户端连接  参数1还是服务端的socket  参数2是接收的客户端的socket  参数3表示客户端socket的长度
	sockaddr_in clientAddr = {};	//初始化客户端的socket地址
	int nAddrLen = sizeof(sockaddr_in);	//初始化一个Int类型的 socket的长度  这个是用来接收客户端的socket的地址长度的
	SOCKET _clientSock = INVALID_SOCKET;		//定义一个无效的socket地址



	char msgBuf[] = "Hello, I,m  server";
	while (true)		//用while（true）包起来就可以接收多个消息了
	{
		_clientSock = accept(_sock, (sockaddr*)&clientAddr,/* (int*)sizeof(clientAddr)*/&nAddrLen);
		if (INVALID_SOCKET == _clientSock)
		{
			//进来表示接收到的socket的连接是无效的
			printf("错误,接收到无效客户端socket。\n");
		}


		printf("新客户端加入:IP=%s \n", (inet_ntoa)(clientAddr.sin_addr));//(inet_ntoa)表示把IP地址转换成可读性的字符串了
		//5 send 想客户端发送一条数据
	//	char msgBuf[] = "Hello, I,m  server";  用while抱起来后  这个写到while外面
		//参数1是要接收信息的客户顿的socket 参数2是要发送对应socket的字符串数据  参数3是数据的长度   +1表示把结尾符一并发送过去    参数4 0表示默认 不知道干嘛
		send(_clientSock, msgBuf, strlen(msgBuf) + 1, 0);
	}
	

		//6.关闭套接字socket  参数是要关闭哪个socket  这里是自己的
		closesocket(_sock);



	//--用Socket API建立简易TCP服务端
	// 1.建立一个socket
	// 2.bind 绑定用户接受客户端连接的网络端口
	// 3.listen 监听网络端口
	// 4.accept 等待接受客户端连接
	// 5.send 向客户端发送一条数据
	// 6.关闭套节字closesocket
	//--------------
	//清楚windows socket 环境


	///

	WSACleanup();


	return 0;
}