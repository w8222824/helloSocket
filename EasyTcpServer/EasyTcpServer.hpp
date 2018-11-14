#ifndef _EasyTcpServer_hpp_					//如果没有定义声成这段代码 

#define  _EasyTcpServer_hpp_				//就生成  



//避免互相依赖的引用  windows里面有一些socke的早期引用  没这个可能会导致宏定义冲突


#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#include <windows.h>
	#include <WinSock2.h>
	#pragma comment(lib,"ws2_32.lib")        // 没有这句话 执行到WSAStartup(ver, &dat);会报无法解析 无法解析的外部符号 __imp__WSAStartup@8的错误   这句话的意思是我需要一个静态链接库  这个只能在windows下才有用 但是我们要跨平台 所以我们右键项目属性点击连接器 在点击输入 在附加依赖项里面添加ws2_32.lib
#else
	#include <unistd.h> //uni std  这个注释表示unix 系统下  的标准库的意思  windows和linux系统都是基于unix系统开发的
	#include <arpa/inet.h>//unix系统下包含的SOCK的库 对应WINSOCK2。H
	#include <string.h>
	#define SOCKET int    //定义linux系统下的socket
	//下面2个就是定义linux系统下的INVALID_SOCKET（这个值是判断socket是否无效的）
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif

#include <stdio.h>    //printf需要导入的头文件
#include<thread>    //c++线程库
#include <vector>    //导入向量  用来存储
#include "MessageHeader.hpp"
#pragma comment(lib,"ws2_32.lib")        // 没有这句话 执行到WSAStartup(ver, &dat);会报无法解析 无法解析的外部符号 __imp__WSAStartup@8的错误   这句话的意思是我需要一个静态链接库  这个只能在windows下才有用 但是我们要跨平台 所以我们右键项目属性点击连接器 在点击输入 在附加依赖项里面添加ws2_32.lib






class EasyTcpServer
{
private:
	SOCKET _sock;
	std::vector<SOCKET> g_clients;        //定义一个全局向量变量来存储客户端的socket

public:	
	EasyTcpServer() {
		_sock = INVALID_SOCKET;//声明一个无效的socket  
	}

	virtual ~EasyTcpServer() {
		Close();
	}

	//初始化Socket
	int InitSocket() {
		//启动Win Sock 2.x环境
#ifdef _WIN32
		//启动Windows socket 2.x环境
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		//--用Socket API建立简易TCP客户端
		// 1.建立一个socket 套接字    怎么建立套接字socket函数  参数1表示ipv4类型的(ipv6是AF_INET6)  参数是什么数据类型的sock  这里是流类型的   参数三是我们要创建什么类型的socket 这里是tcp类型的  函数返回一个套接字

		if (INVALID_SOCKET != _sock)		//如果此时socket是有效的 代表初始化前 对应的socket不是无效的 我们要先关闭掉
		{
			printf("<socket=%d>初始化发现socket是有效的，开始关闭旧连接。\n", (int)_sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//开启连接   IPv4  类型是流  协议传输层是tcp
		if (INVALID_SOCKET == _sock)        //建立套接字socket失败
		{
			printf("错误，建立Socket=<%d>失败。\n", (int)_sock);
		}
		else
		{
			printf("正确，建立Socket=<%d>成功。\n", (int)_sock);
		}

		return  _sock;
	}//InitSocket()


	//绑定IP和端口号
	int Bind(const char* ip ,unsigned short port) {
		//如果初始化socket没有成功
		if (INVALID_SOCKET==_sock)
		{
			InitSocket();	//在初始化一次
		}

		// 2.连接服务器connect  参数1是套接字socket  参数2是sockaddr类型的地址   参数3是地址的长度
		sockaddr_in _sin = {};        //初始化sockaddr 为什么写成sockaddr_in  而不是函数里面需要的sockaddr  是因为sockaddr_in和sockaddr_in是一个类型的  只是sockaddr_in的属性更方便填写我们的代码可以转到定义自己去看下2个的属性区别
		_sin.sin_family = AF_INET;    //协议簇 一般用AF_INET表示TCP/IP协议。  这里表示ipv4
		_sin.sin_port = htons(port)/*4567*/;//为什么不用4567这样的数字  因为我们要转换 host to net unsigned short（主机的数据类型转换成网络的无符号的短字符的数据类型）  这里表示连接到这个端口号的监听

#ifdef _WIN32
		if (ip)					//存在就绑定这个ip  
		{
			_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else
		{
			_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");//sin_addr是一个联合体，用联合体就可以使用多种方式表示IP地址。一般我们使用其中的最后一种方式，即用无符号长整型数据来表示IP地址。此时，用的是s_nu数据成员，s_un是一个结构体，其中只有一个数据成员，即s_addr。           inet_addr("127.0.0.1");    //"127.0.0.1"本机的地址 这么写还能防止外网的访问,  INADDR_ANY表示本机的全部ip地址都可以访问
		}
	
#else
		if (ip)					//存在就绑定这个ip  
		{
			_sin.sin_addr.s_addr = inet_addr(ip);
		}
		else
		{
			_sin.sin_addr.s_addr = INADDR_ANY;// inet_addr("192.168.236.1");	
		}
#endif
		int ret = bind(_sock, (sockaddr*)&_sin, sizeof(_sin));//bind 绑定用于接受客户端连接的网络端口

		if (SOCKET_ERROR == ret)//bind 绑定用于接受客户端连接的网络端口
		{
			//进来表示bind绑定失败
			printf("ERROR,绑定用于接受客户端连接的网络端口<%d>失败，可能被占用。\n",port);
		}
		else
		{
			printf("绑定网络端口<%d>成功。\n", port);

		}

		return 1;	//1在其他语音就是true的意思
	}//Bind()

	//监听端口号  传参n表示服务器最多支持n个连接
	int Listen( int n) 
	{

		// 3.listen 监听网络端口   参数1表示sock  参数2表示绑定的时候最大接受多少人同时连接
		int ret = listen(_sock, n);
		if (SOCKET_ERROR == ret) {

			printf("Socket=<%d>监听网络端口失败。\n", (int)_sock);
		}
		else
		{
			printf("Socket=<%d>监听网络端口成功。\n", (int)_sock);

		}
		return ret;
	}//Listen()

	//接收客户端连接
	SOCKET Accept() {
		sockaddr_in clientAddr = {};    //初始化客户端的socket地址
		int nAddrLen = sizeof(sockaddr_in);    //初始化一个Int类型的 socket的长度  这个是用来接收客户端的socket的地址长度的
		SOCKET _clientSock = INVALID_SOCKET;        //定义一个无效的socket地址
#ifdef _WIN32
		_clientSock = accept((int)_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
		_clientSock = accept((int)_sock, (sockaddr*)&clientAddr, (socklen_t *)&nAddrLen);
#endif
		if (INVALID_SOCKET == _clientSock)
		{
			//进来表示接收到的socket的连接是无效的
			printf("Socket=<%d>错误,接收到无效客户端socket。\n", (int)_sock);
		}
		else
		{
			//for (int n = (int)g_clients.size() - 1; n >= 0; n--)    //这里处理把新客户端加入的通知发送给其他已经加入的客户端
			//{
			//	NewUserJoin newUserJoin;                            //声明用来存储接收新客户端加入通知数据的数据结构
			//	send(g_clients[n], (const char*)&newUserJoin, sizeof(NewUserJoin), 0);        //发送给当前服务器已经知道连接上的客户端
			//}
			NewUserJoin newUserJoin;									//声明用来存储接收新客户端加入通知数据的数据结构
			SendDataToAll(&newUserJoin);
			g_clients.push_back(_clientSock);							//把新加入的客户端存储到全局向量里面
			printf("新客户端加入:socket=%d, IP=%s \n", (int)_clientSock, (inet_ntoa)(clientAddr.sin_addr));//(inet_ntoa)表示把IP地址转换成可读性的字符串了
		}


		return _clientSock;
			
	}

	//关闭Socket
	void Close() {

		if (INVALID_SOCKET != _sock)		//如果连接服务器的时候socket是有效的 我们初始化socket一下
		{

#ifdef _WIN32
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				closesocket(g_clients[n]);

			}
			//8.关闭套接字socket  参数是要关闭哪个socket  这里是自己的
			closesocket(_sock);
			//清楚windows socket 环境
			WSACleanup();
#else
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				close(g_clients[n]);

			}
			//8.关闭套接字socket  参数是要关闭哪个socket  这里是自己的
			close(_sock);
#endif
		}

	}// Close()

	//处理网络消息
	int _nCount = 0;
	bool OnRun() {

		if (isRun())
		{
			//伯克利 socket 描述符  windows 下面 第一个参数没有意义  虽然填写成了_sock+1 但是实际上在windows上不起作用
			fd_set fdRead;    //定义了一个描述符(socket)集合
			fd_set fdWrite;
			fd_set fdExp;
			//下面3个宏是用来fd_set清空
			FD_ZERO(&fdRead);    //清理集合
			FD_ZERO(&fdWrite);
			FD_ZERO(&fdExp);
			//下面的3个宏来设置对应socket对应的有没有可读  可写  异常的操作
			FD_SET(_sock, &fdRead);    //将_sock加入集合
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


			timeval t = { 1,0 };        //select参数5类型是timeval  它是一个时间变量 有个成员变量 一个是秒  一个是微秒  不过windos的计时器好像还不能精确到微秒 所以我们用秒吧 {0,0,}表示不堵塞  不停的执行
										//nfds  是一个整数值  是指fd_set集合中所有的描述符(socket)的范围,而不是数量
										//参数1即是所有文件描述符最大值+1  在windows中这个参数无所谓 可以写0
			int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp,/* NULL*/&t);        //这个select一般来说最后一个数据填写NUll表示是阻塞模式 什么是阻塞模式咧就是当有客户端连接进来它才会有返回值  不然它就一直阻塞到这里 不给返回值  运行一下就知道  直接启动  客户端不连接 它会一直停在这里不往下走  如果不为null就是非堵塞模式了  填进去就表示多长时间去查询一下
			printf("select ret=%d count=%d\n", ret, _nCount++);
			if (ret<0)
			{
				printf("select任务结束\n");
			
				return false;
			}

			if (FD_ISSET(_sock, &fdRead))    //判断描述符(Socket)是否在集合中
			{
				FD_CLR(_sock, &fdRead);        //清理一下  这个清理很鸡贼只是清理了计数器 没有清理对应的数组
				Accept();							   // 4.accept 等待接受客户端连接  参数1还是服务端的socket  参数2是接收的客户端的socket  参数3表示客户端socket的长度


			}


			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				if (FD_ISSET(g_clients[n], &fdRead))
				{
					if (-1 == RecvData(g_clients[n]))	//如果有客户端退出
					{
						/*auto*/std::vector<SOCKET>::iterator iter = g_clients.begin() + n;	//+n才是要移除的迭代器
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

	//是否工作中
	bool  isRun() {

		return _sock != INVALID_SOCKET;
	}

	//接收数据  处理粘包 拆分包
	char szRecv[409600] = {};        //定义一个缓冲区 来接收数据  recv接收到的数据线放到这个缓冲区里面

	int RecvData(SOCKET _cSock) {
					   //5.接收客户端的数据
		int nLen = (int)recv(_cSock, szRecv, /*sizeof(DataHeader)*/409600, 0);    //接收客户端发送过来的数据  参数1对应客户端的socket  参数2 接收客户端发送数据的缓冲区  参数3 缓冲区大小  参数4  0默认
		printf("nLen=%d\n", nLen);
		LoginResult  loginRet;        //因为是变量了 所以不能这么赋予初值了
		SendData(_cSock, &loginRet);
																				  
																				  //DataHeader* header = (DataHeader*)szRecv;        //把每次接收到的数据赋予给header
		//if (nLen <= 0)
		//{
		//	printf("客户端<_clientSock%d>已经退出,任务结束。\n", _cSock);
		//	return -1;
		//}
		//recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);    //参数2数据的容器大小 因为我们现在Login已经把DataHeader给继承了 数据都是一次性收发  所以缓存大小要两者相加（指针的偏移）   参数3参数 因为现在我们已经取到了消息头 要读取的位置也要减少DataHeader这个位置   2-14  06:44  好像这个是指针的内容

		//OnNetMsg(_cSock, header);	//发送数据
		return 0;
	}//int processor(SOCKET _Sock)

	//响应网络消息
	void OnNetMsg(SOCKET _cSock,DataHeader* header) {
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			//参数2就是偏移sizeof(DataHeader)的长度   参数三一样就是dataLength长度-DataHeader的长度
			Login*  login = (Login*)header;        //Login数据结构接收 我们接收到的数据
			printf("收到客户端<_clientSock%d>请求：CMD_LOGIN，数据长度:%d, userName=%s PassWord=%s \n", _cSock, login->dataLength, login->userName, login->PassWord);
			//忽略判断用户密码是否正确过程
			LoginResult  loginRet;        //因为是变量了 所以不能这么赋予初值了
			send(_cSock, (char*)&loginRet, sizeof(LoginResult), 0);    //给对应客户端发送对应的登录结果消息包体
		}
		break;
		case CMD_LOGOUT:
		{
			Logout*  logout = (Logout*)header;        //Logout数据结构接收 我们接收到的数据
			printf("收到收到客户端<_clientSock%d>请求：CMD_LOGOUT  数据长度:%d userName=%s \n", _cSock, logout->dataLength, logout->userName);
			//忽略判断用户密码是否正确过程
			LogoutResult  logoutRet;        //因为是变量了 所以不能这么赋予初值了
			send(_cSock, (char*)&logoutRet, sizeof(LogoutResult), 0);    //给对应客户端发送对应的登录结果消息包体
		}
		break;
		default:
		{
			DataHeader header = { 0, CMD_ERROR };//走到这一步肯定上面2个都不是 直接给找不到的枚举 ERROR  在返回给客户端
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);    //给对应客户端发送对应的登录结果消息包体
		}
		break;
		}
	}//OnNetMsg

	//发送给指定socket数据
	int SendData(SOCKET _cSock,DataHeader* header) {

		if (isRun() && header)	//socket是否在运行  header不为空
		{
			return send(_cSock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}//SendData()

	//给所有连接的socket发送数据
	void SendDataToAll( DataHeader* header) 
	{

		if (isRun() && header)	//socket是否在运行  header不为空
		{
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)    //这里处理把新客户端加入的通知发送给其他已经加入的客户端
			{
				//send(g_clients[n], (const char*)header, header->dataLength, 0);        //发送给当前服务器已经知道连接上的客户端
				SendData(g_clients[n], header);
			}
		}	
		
	}//SendDataToAll()

};

#endif  _EasyTcpServer_hpp_