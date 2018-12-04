//避免互相依赖的引用  windows里面有一些socke的早期引用  没这个可能会导致宏定义冲突
#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32
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

enum   CMD
{
	CMD_LOGIN,                //登录
	CMD_LOGIN_RESULT,        //登录返回
	CMD_LOGOUT,                //登出
	CMD_LOGOUT_RESULT,        //登出返回
	CMD_NEW_USER_JOIN,        //新用户加入
	CMD_ERROR                //错误命令

};

//包头
struct DataHeader
{
	short dataLength;        //数据长度
	short cmd;                //命令


};

struct Login :public DataHeader        //写成继承  然后在结构体的构造函数里面写死包头的数据   就不用分别发送包头和包体了 可以一起发送了
{
	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];

};
//DataPackage  接收服务器返回的数据的包体
struct LoginResult :public DataHeader
{
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};
//新客户端的数据结构
struct NewUserJoin :public DataHeader
{
	NewUserJoin() {
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		socket = 0;
	}
	int socket;
};



//DataPackage  登出数据的包体
struct Logout :public DataHeader
{
	Logout() {
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};
//DataPackage  服务器接收到登出后返回的数据的包体
struct LogoutResult :public DataHeader
{
	LogoutResult() {
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

//定义一个处理函数把处理服务端的逻辑都放进来
int processor(SOCKET _Sock) {

	char szRecv[4096] = {};        //定义一个缓冲区 来接收数据  recv接收到的数据线放到这个缓冲区里面
								   //5.接收服务端的数据
	int nLen = (int)recv(_Sock, szRecv, sizeof(DataHeader), 0);    //接收服务端发送过来的数据  参数1对应客户端的socket  参数2 接收客户端发送数据的缓冲区  参数3 缓冲区大小  参数4  0默认
	DataHeader* header = (DataHeader*)szRecv;        //把每次接收到的数据赋予给header
	if (nLen <= 0)
	{
		printf("与服务器<_Sock%d>断开连接,任务结束。\n", _Sock);
		return -1;
	}
	//if (nLen >= header->dataLength)        //如果进了这个判断才代表至少有一次数据或者以上  而不是少包
	//{

	//}
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		recv(_Sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);    //接收包头是CMD_LOGIN_RESULT的数据
		LoginResult*  loginResult = (LoginResult*)szRecv;        //Logout数据结构接收 我们接收到的数据
		printf("收到服务端<_Sock%d>请求：CMD_LOGIN_RESULT  数据长度:%d 返回的结果:%d\n", _Sock, loginResult->dataLength, loginResult->result);

	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		recv(_Sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);    //接收包头是CMD_LOGOUT_RESULT的数据
		LogoutResult*  logoutResult = (LogoutResult*)szRecv;        //Logout数据结构接收 我们接收到的数据
		printf("收到服务端<_Sock%d>请求：CMD_LOGOUT_RESULT  数据长度:%d 返回的结果:%d\n", _Sock, logoutResult->dataLength, logoutResult->result);

	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		recv(_Sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);    //接收包头是CMD_LOGOUT_RESULT的数据
		NewUserJoin*  newUserJoin = (NewUserJoin*)szRecv;        //Logout数据结构接收 我们接收到的数据
		printf("收到服务端<_Sock%d>请求：CMD_NEW_USER_JOIN  数据长度:%d 新加入的客户端socket:%d\n", _Sock, newUserJoin->dataLength, newUserJoin->socket);

	}
	break;
	}
	return 0;
}//int processor(SOCKET _Sock)

 //cmdThread线程当前是否存在的判断量
bool g_bRunt = true;

//定义一个处理CMD输入的方法
void cmdThread(SOCKET _Sock)
{
	char cmdBuf[256] = {};    //声明一个空间大小为256字符数组
	while (true)
	{
		scanf("%s", cmdBuf);        //控制台输入字符串
		if (0 == strcmp(cmdBuf, "exit"))    //如果输入的字符串为exit
		{
			g_bRunt = false;        //cmdThread线程状态量改为false
			printf("退出cmdThread线程\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.PassWord, "lyd");
			send(_Sock, (const char*)&login, sizeof(Login), 0);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout;
			strcpy(logout.userName, "lyd");

			send(_Sock, (const char*)&logout, sizeof(Logout), 0);
		}
		else
		{
			printf("不支持的命令。\n");
		}
	}


}//void cmdThread(SOCKET _Sock)




int main() {
//
//#ifdef _WIN32
//	//启动Windows socket 2.x环境
//	WORD ver = MAKEWORD(2, 2);
//	WSADATA dat;
//	WSAStartup(ver, &dat);
//#endif
//
//	///
//	//--用Socket API建立简易TCP客户端
//	// 1.建立一个socket 套接字    怎么建立套接字socket函数  参数1表示ipv4类型的(ipv6是AF_INET6)  参数是什么数据类型的sock  这里是流类型的   参数三是我们要创建什么类型的socket 这里是tcp类型的  函数返回一个套接字
//	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
//	if (INVALID_SOCKET == _sock)        //建立套接字socket失败
//	{
//		printf("错误，建立Socket失败。\n");
//	}
//	else {
//		printf("正确，建立Socket成功。\n");
//
//	}
//	// 2.连接服务器connect
//	sockaddr_in _sin = {};
//	_sin.sin_family = AF_INET;            //协议簇 一般用AF_INET表示TCP/IP协议。  这里表示ipv4
//	_sin.sin_port = htons(4567);		 //为什么不用4567这样的数字  因为我们要转换 host to net unsigned short（主机的数据类型转换成网络的无符号的短字符的数据类型）  客户端表示连接到服务端的4567端口号
//#ifdef _WIN32
//	_sin.sin_addr.S_un.S_addr = inet_addr("192.168.81.1");//sin_addr是一个联合体，用联合体就可以使用多种方式表示IP地址。一般我们使用其中的最后一种方式，即用无符号长整型数据来表示IP地址。此时，用的是s_nu数据成员，s_un是一个结构体，其中只有一个数据成员，即s_addr。           inet_addr("127.0.0.1");    //"127.0.0.1"本机的地址 这么写还能防止外网的访问,  INADDR_ANY表示本机的全部ip地址都可以访问
//#else
//	_sin.sin_addr.s_addr = inet_addr("192.168.236.1");
//#endif
//	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));    //连接对应的socket    参数3长度还是传类型比较好
//	if (SOCKET_ERROR == ret)				//建立套接字socket失败
//	{
//		printf("错误，连接服务器失败失败。\n");
//	}
//	else {
//		printf("正确，连接服务器成功。\n");
//	}
	//启动线程
	std::thread t1(cmdThread, _sock);            //声明一个线程专门用来执行传参的方法  参数2是参数1方法所要的传参
	t1.detach();								//线程和主线程分离
	while (g_bRunt)								//输入线程都不存在了 下面的sock接收处理逻辑也不该继续执行了
	{
		//客户端只需要读  不需要写和异常和服务端有区别
		fd_set fdRead;            //定义队列集合貌似 大小最大貌似能装64个socket
		FD_ZERO(&fdRead);        //清理  就是个数清理 数组没请
		FD_SET(_sock, &fdRead); //向集合添加描述字。
		timeval t = { 1,0 };        //select参数5类型是timeval  它是一个时间变量 有个成员变量 一个是秒  一个是微秒  不过windos的计时器好像还不能精确到微秒 所以我们用秒吧 {0,0,}表示不堵塞  不停的执行

		//参数1时所有文件描述符最大值+1。在windows中这个参数可以写0 如果其他系统不写最大值+1会收不到服务器返回的数据
		int ret = select(_sock + 1, &fdRead, 0, 0, &t);    //参数3  4 是写  和异常的 我们暂时不需要传0  最后一个参数传的是0问默认 感觉效果和NULL一样

		if (ret<0)
		{
			printf("select任务结束\n");
			break;
		}

		if (FD_ISSET(_sock, &fdRead))    //判断在FD_SET集合里面有没有这个描述符socket 可读数据 说白就是是这个接收到的数据 是否正常
		{
			//进来表示集合里面有这个对应的_SOCK  我们清理一下
			FD_CLR(_sock, &fdRead);


			//开始处理这个_sock发送过来的数据  进入下面的判断表示处理的数据不是我们想要处理的数据
			if (-1 == processor(_sock))
			{
				printf("select任务结束，与服务器断开连接\n");
				break;
			}
		}


		//    printf("客户端空闲时间处理其他业务。。select为非堵塞模式该打印会不断执行\n");


	}

//	// 7.关闭套节字closesocket
//#ifdef _WIN32
//	closesocket(_sock);
//	//清除Windows socket环境
//	WSACleanup();
//#else
//	close(_sock);
//#endif
	printf("已退出，任务结束\n");
	getchar();
	return 0;
}
