
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


//客户端连接的最大数量
const int  cCount = 10000;		//已经在EasyTcpServer里面将WINSOCKE2.H里面的FD_SETSIZE从64自定义成了1024
								//	const int  cCount = 1;		//FD_SETSIZE最大连接数 64   -1 表示服务器的那个
//发送线程数量
const int  tCount = 4;

//声明一个数组作为客户端数组  大小为10		
EasyTcpClient* client[cCount];			

//定义发送线程
void sendThread(int id)
{
	//目前发送线程 就4个 ID就是 1~4
	int c = cCount / tCount;	//每个发送线程要连接的客户端总数
	int begin = (id-1)*c;		//这个算出每个线程连接的客户端书从哪里开始
	int  end = id * c;			//这个算出每个线程连接的客户端数到哪结束

	for (size_t i = begin; i < end; i++)
	{
		client[i] = new EasyTcpClient();
	}

	for (size_t i = begin; i < end; i++)
	{

		client[i]->Connect("127.0.0.1", 4567);			//全部创建完了后再开始连接
		printf("thread<%d> Connect=%d\n",id, i);


	}

	std::chrono::milliseconds t(5000);// 声明  c++标准库里面的  时间变量 t为 3000毫秒   
	std::this_thread::sleep_for(t);// 开启一个线程 休眠t秒   跨平台的 这个



	//client.InitSocket();
	//client1.Connect("172.16.47.190", 4567);				// 阿里云  私网ip
	//client1.Connect("47.98.215.178", 4567);				// 阿里云  公网ip
	//client1.Connect("127.0.0.1", 4567);				// 本地地址
	//client1.Connect("192.168.81.1", 4567);		//windows下的ipv4  本地地址
	//	client1.Connect("192.168.236.130", 4567);		//mac下的ipv4  本地地址
	//client1.Connect("192.168.236.133", 4567);		//ubuntu下的ipv4 本地地址

	Login login[10];
	for (size_t i = 0; i < 10; i++)
	{
		strcpy(login[i].userName, "lyd");
		strcpy(login[i].PassWord, "lydmm");
	}
	const int nLen = sizeof(login);
	while (/*client1.isRun()&& */g_bRunt)								//输入线程都不存在了 下面的sock接收处理逻辑也不该继续执行了
	{
		for (int i = 0; i < cCount; i++)
		{
			client[i]->SendData(&login[i], nLen);			//循环发送数据
		//	client[i]->OnRun();					//处理
		}
	}

	for (int i = 0; i < cCount; i++)
	{
		client[i]->Close();
	}
}


int main() {

		//启动UI线程
		std::thread t1(cmdThread/*, &client1*/);            //声明一个线程专门用来执行传参的方法  参数2是参数1方法所要的传参
		t1.detach();										//线程和主线程分离
		
		//启动发送线程
		for (size_t i = 0; i < tCount; i++)
		{
			std::thread t1(sendThread,i+1);
			t1.detach();//线程和主线程分离
		}



		while (g_bRunt)
		{
			Sleep(100);
		}




	printf("已退出，任务结束\n");
	getchar();
	return 0;
}
