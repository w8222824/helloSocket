
#include<thread>    //c++线程库
#include "EasyTcpClient.hpp"



//cmdThread线程当前是否存在的判断量
bool g_bRunt = true;

//定义一个处理CMD输入的方法
void cmdThread(EasyTcpClient* client)
{

	while (true)
	{
		char cmdBuf[256] = {};    //声明一个空间大小为256字符数组

		scanf("%s", cmdBuf);        //控制台输入字符串
		if (0 == strcmp(cmdBuf, "exit"))    //如果输入的字符串为exit
		{
			client->Close();
			printf("退出cmdThread线程\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.PassWord, "lydmm");
			client->SendData(&login);

		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout;
			strcpy(logout.userName, "lyd");
			client->SendData(&logout);
		}
		else
		{
			printf("不支持的命令。\n");
		}
	}


}//void cmdThread(SOCKET _Sock)




int main() {
	EasyTcpClient client;
	//client.InitSocket();
	client.Connect("127.0.0.1", 4567);


	//启动UI线程
	std::thread t1(cmdThread, &client);            //声明一个线程专门用来执行传参的方法  参数2是参数1方法所要的传参
	t1.detach();									//线程和主线程分离


	while (/*g_bRunt*/client.isRun())								//输入线程都不存在了 下面的sock接收处理逻辑也不该继续执行了
	{
		client.OnRun();
	
	}

	client.Close();


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
