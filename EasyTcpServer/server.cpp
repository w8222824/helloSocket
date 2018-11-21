

#include "EasyTcpServer.hpp"
#include <thread>

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

	server.Listen(50);			//监听 表示最多监听5个客户端
	

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
