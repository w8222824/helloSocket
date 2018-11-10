#ifndef _EasyTcpClient_hpp_					//如果没有定义声成这段代码 就生成  
#define _EasyTcpClient_hpp_					//这么写保证只会被引用一次

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
#include <stdio.h>							//printf需要导入的头文件
#include "MessageHeader.hpp"

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
		Close();
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
			printf("<socket=%d>初始化发现socket是有效的，开始关闭旧连接。\n", _sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//开启连接   IPv4  类型是流  协议传输层是tcp
		if (INVALID_SOCKET == _sock)        //建立套接字socket失败
		{
			printf("错误，建立Socket失败。\n");
		}
		else
		{
			printf("正确，建立Socket=<%d>成功。\n",_sock);
		}
		return 0;
	}

	//连接服务器 参数1 ip  参数2  端口号
	int Connect(const char* ip,unsigned short port)
	{
		if (INVALID_SOCKET == _sock)		//如果连接服务器的时候socket是无效的 我们初始化socket一下
		{
			InitSocket();
		}
		// 2.连接服务器connect
		 sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;            //协议簇 一般用AF_INET表示TCP/IP协议。  这里表示ipv4
		_sin.sin_port = htons(port);		 //为什么不用4567这样的数字  因为我们要转换 host to net unsigned short（主机的数据类型转换成网络的无符号的短字符的数据类型）  客户端表示连接到服务端的4567端口号
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);//sin_addr是一个联合体，用联合体就可以使用多种方式表示IP地址。一般我们使用其中的最后一种方式，即用无符号长整型数据来表示IP地址。此时，用的是s_nu数据成员，s_un是一个结构体，其中只有一个数据成员，即s_addr。           inet_addr("127.0.0.1");    //"127.0.0.1"本机的地址 这么写还能防止外网的访问,  INADDR_ANY表示本机的全部ip地址都可以访问
#else
		_sin.sin_addr.s_addr = inet_addr(ip);
#endif
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));    //连接对应的socket    参数3长度还是传类型比较好
		if (SOCKET_ERROR == ret)				//建立套接字socket失败
		{
			printf("<socket=%d>错误，连接服务器<%s:%d>失败。\n",_sock,ip,port);
		}
		else {
			printf("<socket=%d>正确，连接服务器<%s:%d>成功。\n", _sock, ip, port);
		}
		return ret;
	}

	//关闭套接字  closesocket
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
	bool  OnRun() {


		if (isRun())			//判断socket是否在运行
		{
			fd_set fdReads;		//定义了一个可读的描述符(socket)集合  
			FD_ZERO(&fdReads);			//然后清理这个描述符集合
			FD_SET(_sock, &fdReads);	//将参数1_sock添加到这个可读描述符集合里面

			timeval t = { 0,0 };		//定义一个 时间变量类  这个表示1秒   表示socket多久处理一次
			int ret = select(_sock + 1, &fdReads, 0, 0, &t);		//创建了一个socket连接 参数1最大socket最大值+1  参数2是可读的套接字结合  参数3是可写的套接字集合，不需要填0默认  参数4异常的套接字集合 不需要  填0  参数5 每次间隔时间  这里是1秒
			if (ret < 0)	//小于0一定是连接失败了  
			{
				printf("<socket=%d>select任务结束1\n", _sock);
				Close();
				return false;
			}

			if (FD_ISSET(_sock, &fdReads))		//判断这个sock  是否在可读的描述符集合里面
			{
				FD_CLR(_sock, &fdReads);	//清理一下  这个清理很鸡贼只是清理了计数器 没有清理对应的数组

				if (-1 == RecvData(_sock))		//连接断开
				{
					printf("<socket=%d>select任务结束2\n", _sock);
					Close();
					return false;
				}
			}
			return true;				//返回true  表示前面的都没有问题 创建连接成功  也成功和服务器连接没有断开 
		}
		return false;			//缺少的
	}//OnRun()

	 //是否在运行中   true表示运行 否者没有运行
	bool  isRun() {

		return _sock != INVALID_SOCKET;
	}


	//接收数据	处理粘包 拆包 				定义一个处理函数把处理服务端的逻辑都放进来
	int RecvData(SOCKET _cSock) {

		char szRecv[4096] = {};       //定义一个缓冲区 来接收数据  recv接收到的数据线放到这个缓冲区里面
									   //5.接收服务端的数据
		int nLen = (int)recv(_cSock, szRecv, sizeof(DataHeader), 0);    //接收服务端发送过来的数据  参数1对应客户端的socket  参数2 接收客户端发送数据的缓冲区  参数3 缓冲区大小  参数4  0默认
		DataHeader* header = (DataHeader*)szRecv;        //把每次接收到的数据赋予给header
		if (nLen <= 0)
		{
			printf("与服务器<_Sock%d>断开连接,任务结束。\n", _cSock);
			return -1;
		}

		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);    //接收包头是CMD_LOGIN_RESULT的数据

		OnNetMsg(header);
		return 0;
	}//int processor(SOCKET _Sock)


	 //响应网络消息
	void OnNetMsg(DataHeader* header) {


		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResult* login = (LoginResult*)header;        //Logout数据结构接收 我们接收到的数据
			 //	printf("收到服务端<_Sock%d>请求：CMD_LOGIN_RESULT  数据长度:%d 返回的结果:%d\n", _sock, loginResult->dataLength, loginResult->result);
			printf("<socket=%d>收到服务端消息：CMD_LOGIN_RESULT,数据长度：%d\n",_sock, login->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			LogoutResult* logout = (LogoutResult*)header;     //Logout数据结构接收 我们接收到的数据
			//	printf("收到服务端<_Sock%d>请求：CMD_LOGOUT_RESULT  数据长度:%d 返回的结果:%d\n", _sock, logoutResult->dataLength, logoutResult->result);
			printf("<socket=%d>收到服务端消息：CMD_LOGOUT_RESULT,数据长度：%d\n", _sock, logout->dataLength);

		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* userJoin = (NewUserJoin*)header;        //Logout数据结构接收 我们接收到的数据
			 //	printf("收到服务端<_Sock%d>请求：CMD_NEW_USER_JOIN  数据长度:%d 新加入的客户端socket:%d\n", _sock, newUserJoin->dataLength, newUserJoin->socket);
			printf("<socket=%d>收到服务端消息：CMD_NEW_USER_JOIN,数据长度：%d\n", _sock, userJoin->dataLength);

		}
		break;
		}
	}//OnNetMsg

	 //发送数据
	int SendData(DataHeader* header) {

		if (isRun() && header)	//socket是否在运行  header不为空
		{
			return send(_sock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}


private:

};

#else
//已经被定义过了
#endif