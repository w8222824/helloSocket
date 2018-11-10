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

int main() {

	EasyTcpServer server;		//声明
	server.InitSocket();		//初始化socket 就是创建socket
	server.Bind(nullptr,4567);	//绑定任意ip的 4567端口号

	server.Listen(5);			//监听 表示最多监听5个客户端
	

	while (server.isRun())        //判断服务器socket是否存在
	{
		server.OnRun();		//处理接收到的消息
		
	}

	server.Close();

	printf("已退出，任务结束\n");
	getchar();

	return 0;
}
