#ifndef _EasyTcpClient.hpp_					//如果没有定义声成这段代码 就生成  
#define _EasyTcpClient.hpp_					//这么写保证只会被引用一次

#ifdef _WIN32

	#define WIN32_LEAN_AND_MEAN				//避免互相依赖的引用  windows里面有一些socke的早期引用  没这个可能会导致宏定义冲突
	#include <windows.h>
	#include <WinSock2.h>
	#pragma comment(lib,"ws2_32.lib")		// 没有这句话 执行到WSAStartup(ver, &dat);会报无法解析 无法解析的外部符号 __imp__WSAStartup@8的错误   这句话的意思是我需要一个静态链接库  这个只能在windows下才有用 但是我们要跨平台 所以我们右键项目属性点击连接器 在点击输入 在附加依赖项里面添加ws2_32.lib

#else
	#include <unistd.h>						//uni std  这个注释表示unix 系统下  的标准库的意思  windows和linux系统都是基于unix系统开发的
	#include <arpa/inet.h>					//unix系统下包含的SOCK的库 对应WINSOCK2。H
	#include <string.h>
	#define SOCKET int						//定义linux系统下的socket
											//下面2个就是定义linux系统下的INVALID_SOCKET（这个值是判断socket是否无效的）
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif

#include <stdio.h>    //printf需要导入的头文件

class EasyTcpClient
{
	SOCKET _sock;					//声明一个socket  套节字
public:
	//构造函数
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;//声明一个无效的socket  

	}


	//虚析构函数
	virtual ~EasyTcpClient()
	{
		closesocket(_sock);
	};

	//初始化socket
	int InitSocket() 
	{
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
			printf("<socket=%d>初始化发现socket是有效的，开始关闭旧连接。\n",_sock);
			Close();	
		}
		
		
		_sock = socket(AF_INET, SOCK_STREAM, /*0*/IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)        //建立套接字socket失败
		{
			printf("错误，建立Socket失败。\n");
		}
		else 
		{
			printf("正确，建立Socket成功。\n");
		}

	}

	//连接服务器
	int Connect(char* ip, unsigned short port) 
	{
		if (INVALID_SOCKET == _sock)		//如果连接服务器的时候socket是无效的 我们初始化socket一下
		{
			InitSocket();
		}
		// 2.连接服务器connect
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;            //协议簇 一般用AF_INET表示TCP/IP协议。  这里表示ipv4
		_sin.sin_port = htons(4567);		 //为什么不用4567这样的数字  因为我们要转换 host to net unsigned short（主机的数据类型转换成网络的无符号的短字符的数据类型）  客户端表示连接到服务端的4567端口号
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);//sin_addr是一个联合体，用联合体就可以使用多种方式表示IP地址。一般我们使用其中的最后一种方式，即用无符号长整型数据来表示IP地址。此时，用的是s_nu数据成员，s_un是一个结构体，其中只有一个数据成员，即s_addr。           inet_addr("127.0.0.1");    //"127.0.0.1"本机的地址 这么写还能防止外网的访问,  INADDR_ANY表示本机的全部ip地址都可以访问
#else
		_sin.sin_addr.s_addr = inet_addr(ip);
#endif
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));    //连接对应的socket    参数3长度还是传类型比较好
		if (SOCKET_ERROR == ret)				//建立套接字socket失败
		{
			printf("错误，连接服务器失败失败。\n");
		}
		else {
			printf("正确，连接服务器成功。\n");
		}
		return ret;
	}

	//关闭socket
	void Close()
	{
		//如果socket不是无效的 才执行下面的
		if (_sock != INVALID_SOCKET) 
		{
			//关闭套节字closesocket
#ifdef _WIN32
			closesocket(_sock);
			//清除Windows socket环境
			WSACleanup();
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;//关闭清理完毕后 赋予无效值
		}
	


	}


	//发送数据
	//接收数据
	//处理网络消息

private:

};

#else
		//已经被定义过了
#endif