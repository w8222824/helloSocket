
#include<thread>    //c++线程库
#include "EasyTcpClient.hpp"



//cmdThread线程当前是否存在的判断量
bool g_bRunt = true;

//定义一个处理CMD输入的方法
void cmdThread(/*EasyTcpClient* client*/)
{

	while (true)
	{

		char cmdBuf[256] = {};    //声明一个空间大小为256字符数组

		scanf("%s", cmdBuf);        //控制台输入字符串
		if (0 == strcmp(cmdBuf, "exit"))    //如果输入的字符串为exit
		{
			//client->Close();
			 g_bRunt = false;
			printf("退出cmdThread线程\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.PassWord, "lydmm");
			//client->SendData(&login);

		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout;
			strcpy(logout.userName, "lyd");
			//client->SendData(&logout);
		}
		else
		{
			printf("不支持的命令。\n");
		}
	}


}//void cmdThread(SOCKET _Sock)




int main() {
	//const int  cCount = FD_SETSIZE-1;		//FD_SETSIZE最大连接数 64   -1 表示服务器的那个
		const int  cCount = 1;		//FD_SETSIZE最大连接数 64   -1 表示服务器的那个
	EasyTcpClient* client[cCount];			//声明一个数组  大小为10		
	int  a = sizeof(EasyTcpClient);
	for (int i = 0; i < cCount; i++)
	{
	//	client[i].Connect("47.98.215.178", 4567);				//启动多个客户端连接服务端   阿里云  公网ip
		client[i] = new EasyTcpClient();
	//	client[i]->Connect("127.0.0.1", 4567);				// 本地地址
		client[i]->Connect("47.98.215.178", 4567);		
		//client[i]->Connect("192.168.158.155", 4567);		//ubuntu下的ipv4 本地地址
	}

	//client.InitSocket();
	//client1.Connect("172.16.47.190", 4567);				// 阿里云  私网ip
	//client1.Connect("47.98.215.178", 4567);				// 阿里云  公网ip
	//client1.Connect("127.0.0.1", 4567);				// 本地地址
	//client1.Connect("192.168.81.1", 4567);		//windows下的ipv4  本地地址
//	client1.Connect("192.168.236.130", 4567);		//mac下的ipv4  本地地址
	//client1.Connect("192.168.236.133", 4567);		//ubuntu下的ipv4 本地地址

	//启动UI线程
	std::thread t1(cmdThread/*, &client1*/);            //声明一个线程专门用来执行传参的方法  参数2是参数1方法所要的传参
		t1.detach();									//线程和主线程分离



	Login login;
	strcpy(login.userName, "lyd");
	strcpy(login.PassWord, "lydmm");
	while (/*client1.isRun()&& */g_bRunt)								//输入线程都不存在了 下面的sock接收处理逻辑也不该继续执行了
	{
		//client1.OnRun();			暂时注销 不处理数据了
		//client1.SendData(&login);
		for (int i = 0; i < cCount; i++)
		{
					
			client[i]->SendData(&login);			//循环发送数据
			client[i]->OnRun();					//处理
		}
	}

	for (int i = 0; i < cCount; i++)
	{
		client[i]->Close();
	}
	//client1.Close();


	printf("已退出，任务结束\n");
	getchar();
	return 0;
}
