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

#pragma comment(lib,"ws2_32.lib")        // 没有这句话 执行到WSAStartup(ver, &dat);会报无法解析 无法解析的外部符号 __imp__WSAStartup@8的错误   这句话的意思是我需要一个静态链接库  这个只能在windows下才有用 但是我们要跨平台 所以我们右键项目属性点击连接器 在点击输入 在附加依赖项里面添加ws2_32.lib


#include "EasyTcpServer.hpp"


bool  g_bRun = true;			//控制服务器是否退出的状态量
//定义一个处理CMD输入的方法
void cmdThread()
{

	while (true)
	{

		char cmdBuf[256] = {};    //声明一个空间大小为256字符数组

		scanf("%s", cmdBuf);        //控制台输入字符串
		if (0 == strcmp(cmdBuf, "exit"))    //如果输入的字符串为exit
		{	
			
	
			g_bRun = false;
			printf("退出cmdThread线程\n");
			break;
		}
		else
		{
			printf("不支持的命令。\n");
		}
	}


}//void cmdThread(SOCKET _Sock)




int main() {

	EasyTcpServer server;		//声明
	server.InitSocket();		//初始化socket 就是创建socket
	server.Bind(nullptr,4567);	//绑定任意ip的 4567端口号

	server.Listen(5);			//监听 表示最多监听5个客户端
	

								//启动UI线程
	std::thread t1(cmdThread);            //声明一个线程专门用来执行传参的方法  参数2是参数1方法所要的传参
	t1.detach();									//线程和主线程分离



	while (server.isRun()&& g_bRun)        //判断服务器socket是否存在  且 g_bRun为true 
	{
		server.OnRun();		//处理接收到的消息
		
	}

	server.Close();

	printf("已退出，任务结束\n");
	getchar();

	return 0;
}
