#ifndef _EasyTcpServer_hpp_					//如果没有定义声成这段代码 

#define  _EasyTcpServer_hpp_				//就生成  



//避免互相依赖的引用  windows里面有一些socke的早期引用  没这个可能会导致宏定义冲突


#ifdef _WIN32
	#define FD_SETSIZE   2506					//这个在winsock2.h里面是 如果没有定义  就定义成64  我们这边自己定义成1024  避开windows下 单线程最多64个socket的连接限制
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
#include<mutex>			//锁的
#include<atomic>


#include "MessageHeader.hpp"
#include "CELLTimestamp.hpp"
#pragma comment(lib,"ws2_32.lib")        // 没有这句话 执行到WSAStartup(ver, &dat);会报无法解析 无法解析的外部符号 __imp__WSAStartup@8的错误   这句话的意思是我需要一个静态链接库  这个只能在windows下才有用 但是我们要跨平台 所以我们右键项目属性点击连接器 在点击输入 在附加依赖项里面添加ws2_32.lib


#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240			// 使用宏定义一个字符串缓冲区最小单元大小  这里RECV_BUFF_SIZE表示整数10240  10Kb大小应该
#endif

#define _CellServer_THREAD_COUNT 4			

//----------------------------------------------------------------------------------------------------------------------------------------------
//客户端数据类型
class ClientSocket 
{
public:
	ClientSocket(SOCKET sockfd=INVALID_SOCKET)
	{
		_sockfd = sockfd;
		memset(_szMsgBuf, 0, sizeof(_szMsgBuf));
		_lastPos = 0;
	}
	SOCKET sockfd() {
		return _sockfd;
	}
	//数据源  也就是从系统缓冲区里面瓦水的水瓢
	char* msgBuf() {
		return _szMsgBuf;
	}

	int getLastPos() {

		return _lastPos;
	}


	void setLastPos( int lastpos) {
		_lastPos = lastpos;
	}

	//发送数据
	int SendData( DataHeader* header)
	{
		if ( header)
		{
			return send(_sockfd, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}



private:
	SOCKET _sockfd;		//fd_set    file desc  set
	char _szMsgBuf[RECV_BUFF_SIZE*5] ;		//定义一个第二缓冲区  消息缓冲区
	int _lastPos;
};//class ClientSocket 

//网络事件接口
class INetEvent
{
public:
	//纯虚函数 就是继承了INetEvent  这个类的 都需要自己实现
	//客户端离开事件
	virtual void OnNetJoin(ClientSocket* pClient) = 0;

	//客户端离开事件
	virtual void OnNetLeave(ClientSocket* pClient) = 0;
	//客户端消息事件
	virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header) = 0;
private:

};

//-----------------------------------------------------------------------------------------------------------------------------------------------

//小型服务类
class CellServer
{
public:
	CellServer(SOCKET sock=INVALID_SOCKET) {
		_sock = sock;
	
		_pNetEvent = nullptr;
	}
	~CellServer() {
		
		Close();
		_sock = INVALID_SOCKET;
	}



	//设置事件Obj
	void setEventObj(INetEvent* event)		
	{
		_pNetEvent = event;
	}

	//关闭Socket
	void Close() {

		if (INVALID_SOCKET != _sock)		//如果连接服务器的时候socket是有效的 我们初始化socket一下
		{

#ifdef _WIN32
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				closesocket(_clients[n]->sockfd());
				delete _clients[n];	//new了对象 我们需要删除掉

			}
			//8.关闭套接字socket  参数是要关闭哪个socket  这里是自己的
			closesocket(_sock);
			//清楚windows socket 环境
			//WSACleanup();
#else
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				close(_clients[n]->sockfd());
				delete _clients[n];	//new了对象 我们需要删除掉

			}
			//8.关闭套接字socket  参数是要关闭哪个socket  这里是自己的
			close(_sock->sockfd());

#endif
			_clients.clear();		//清理用来存储客户端套接字socket的数组
		}

	}// Close()

	//是否工作中
	bool  isRun() {

		return _sock != INVALID_SOCKET;
	}

	//int _nCount = 0;
	//处理网络消息
	bool OnRun() {

		while (isRun())
		{
			if (_clientsBuff.size()>0)
			{
				//从缓冲队列里取出客户数据
				std::lock_guard<std::mutex> lock(_mutex);
				for (auto pClient : _clientsBuff)
				{
					_clients.push_back(pClient);
				}
				_clientsBuff.clear();
			}
		
			//如果没有需要处理的客户端，就跳过
			if (_clients.empty())
			{
				std::chrono::milliseconds t(1);// 声明  c++标准库里面的  时间变量 t为 1毫秒   
				std::this_thread::sleep_for(t);// 开启一个线程 休眠t秒   跨平台的 这个
				continue;//结束本次循环 
			}

			//伯克利套接字 BSD socket
			fd_set fdRead;//描述符（socket） 集合
			//清理集合
			FD_ZERO(&fdRead);
			//将描述符（socket）加入集合
			SOCKET maxSock = _clients[0]->sockfd();
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				FD_SET(_clients[n]->sockfd(), &fdRead);
				if (maxSock < _clients[n]->sockfd())
				{
					maxSock = _clients[n]->sockfd();
				}
			}
			///nfds 是一个整数值 是指fd_set集合中所有描述符(socket)的范围，而不是数量
			///既是所有文件描述符最大值+1 在Windows中这个参数可以写0
			int ret = select(maxSock + 1, &fdRead, nullptr, nullptr, nullptr);
			if (ret < 0)
			{
				printf("select任务结束。\n");
				Close();
				return false;
			}

			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				if (FD_ISSET(_clients[n]->sockfd(), &fdRead))
				{
					if (-1 == RecvData(_clients[n]))		//当==-1时候客户端肯定退出了
					{
						auto iter = _clients.begin() + n;//std::vector<SOCKET>::iterator
						if (iter != _clients.end())
						{
							if (_pNetEvent)
								_pNetEvent->OnNetLeave(_clients[n]);
							delete _clients[n];
							_clients.erase(iter);
						}

						
					}
				}
			}
		}//if (isRun())

	}//bool OnRun()

	char _szRecv[RECV_BUFF_SIZE] = {};        //定义一个缓冲区 来接收数据  recv接收到的数据线放到这个缓冲区里面

	//接收数据  处理粘包 拆分包
	int RecvData(ClientSocket* pClient) {
		//5.接收客户端的数据
		int nLen = (int)recv(pClient->sockfd(), _szRecv, /*sizeof(DataHeader)*/RECV_BUFF_SIZE, 0);    //接收客户端发送过来的数据  参数1对应客户端的socket  参数2 接收客户端发送数据的缓冲区  参数3 缓冲区大小  参数4  0默认																															  //DataHeader* header = (DataHeader*)szRecv;        //把每次接收到的数据赋予给header
		if (nLen <= 0)
		{
		//	printf("客户端<_clientSock%d>已经退出,任务结束。\n", pClient->sockfd());
			return -1;
		}
		//将收取到的数据拷贝到消息缓冲区
		memcpy(pClient->msgBuf() + pClient->getLastPos(), _szRecv, nLen);				//将每次接收数据放到我们定义的第二缓冲区_szMsgBuf里面（就是_szRecv这个源就是从recv的系统接收缓冲区取出数据的瓢子大小,nLen就是每次接收到的数据实际的大小  然后我们把每次接收到的实际数据nLen放到我们定义的第二缓冲区_szMsgBuf里面）     或者说_szRecv里面有从系统缓冲区瓦出来的数据  可能没满可能满了 我们取nLen 这个长度的数据 然后复制到_szMsgBuf 这个第二缓冲区里面

		pClient->setLastPos(pClient->getLastPos() + nLen);							//记录这一次消息的放了多长 下一次在这个后面再放  防止消息被覆盖了
		while (pClient->getLastPos() >= sizeof(DataHeader))				//如果消息缓冲区的数据已经大于一个消息头DataHeader长度,这时就可以知道当前消息体的长度  因为接受的数据里面已经至少有一个完整消息头的数据量了
		{

			DataHeader* header = (DataHeader*)pClient->msgBuf();	//把当前的消息数据强转成DataHeader类型

			if (pClient->getLastPos() >= header->dataLength)			//当前消息缓冲区的数据长度大于了消息长度
			{
				int nSize = pClient->getLastPos() - header->dataLength;//定义一个剩余未处理消息缓冲区的数据长度    处理前先存储一下这个值 因为处理后这个dataLength值已经发生了改变了 

				OnNetMsg(pClient, header);						//使用我们封装的接口处理网络消息
				//将剩余的未处理的数据前移
				memcpy(/*_szMsgBuf*/pClient->msgBuf(), /*_szMsgBuf*/pClient->msgBuf() + header->dataLength,/* _lastPos*/pClient->getLastPos() - header->dataLength);	//处理后我们需要把处理过的数据拿出来  没有处理的数据往前移,填充满这里面的数据  （参数1还是要接收数据的缓冲区_szMsgBuf  ,参数2表示数据源从_szMsgBuf的 header->dataLength的位置开始,这个之前取过了所以我们不需要了, 参数3_lastPos- header->dataLength，_lastPos是目前_szMsgBuf里面存储的数据的长度 我们上一步已经处理了一次了,所以我们我们实际缓存里面应该存储的长度是（总的）-（已经处理过的）= _lastPos- header->dataLength  的长度  这个长度）
				pClient->setLastPos(nSize);		/*_lastPos = nSize;*/						//写成_lastPos =_lastPos - header->dataLength   肯定不对的 因为上一步处理过了 

			}
			else
			{
				//消息缓冲区的消息目前剩余的消息不够一条完整的消息  退出该次while循环
				break;
			}
		}
		return 0;
	}//int processor(SOCKET _Sock)

	//响应网络消息
	virtual void OnNetMsg(ClientSocket*  pClient, DataHeader* header) {

		//_recvCount++;
		_pNetEvent->OnNetMsg(pClient, header); // 计算时间的  更准 但是有耗时或者说消耗


		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			//参数2就是偏移sizeof(DataHeader)的长度   参数三一样就是dataLength长度-DataHeader的长度
			Login*  login = (Login*)header;        //Login数据结构接收 我们接收到的数据
			//printf("收到客户端<_clientSock%d>请求：CMD_LOGIN，数据长度:%d, userName=%s PassWord=%s \n", cSock, login->dataLength, login->userName, login->PassWord);
			//忽略判断用户密码是否正确过程
			LoginResult  loginRet;        //因为是变量了 所以不能这么赋予初值了
			//send(cSock, (char*)&loginRet, sizeof(LoginResult), 0);    //给对应客户端发送对应的登录结果消息包体
			//SendData(cSock, &loginRet);
			pClient->SendData(&loginRet);
		}
		break;
		case CMD_LOGOUT:
		{
			Logout*  logout = (Logout*)header;        //Logout数据结构接收 我们接收到的数据
			//printf("收到收到客户端<_clientSock%d>请求：CMD_LOGOUT  数据长度:%d userName=%s \n", cSock, logout->dataLength, logout->userName);
			//忽略判断用户密码是否正确过程
			LogoutResult  logoutRet;        //因为是变量了 所以不能这么赋予初值了
			//send(cSock, (char*)&logoutRet, sizeof(LogoutResult), 0);    //给对应客户端发送对应的登录结果消息包体
		//	SendData(cSock, &logoutRet);
		}
		break;
		default:
		{
			printf("<socket=%d>收到未定义数据请求，数据长度:%d  \n", pClient->sockfd(), header->dataLength);

			DataHeader header; //走到这一步肯定上面2个都不是 直接给找不到的枚举 ERROR  在返回给客户端
			//send(cSock, (char*)&header, sizeof(DataHeader), 0);    //给对应客户端发送对应的登录结果消息包体
			//SendData(cSock, &header);
		}
		break;
		}
	}//OnNetMsg


		//发送指定Socket数据
	int SendData(SOCKET cSock, DataHeader* header)
	{
		if (isRun() && header)
		{
			return send(cSock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}
	

	void addClient(ClientSocket* pClient)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		//_mutex.lock();
		_clientsBuff.push_back(pClient);
		//_mutex.unlock();
	}

	//开启处理网络消息的线程
	void Start()
	{
		//std::thread t(&CellServer::OnRun, this);	//这个写法会被强转  下面的写法更新安全
		_Thread = std::thread(std::mem_fn(&CellServer::OnRun), this);	//mem_fun是成员方法把一个成员方法转换成方法对象,使用对象指针进行绑定   this表示线程启动了一个无参方法  c++在编译的时候会给成员函数传一个this指针参数
																		//mem_fn是成员方法把一个成员方法转换成方法对象,使用对象指针或者引用进行绑定	
	}			

	//获取客户端连接数的  当前客户端的数量和 缓存的客户端的数量
	size_t getClientCount()
	{
		return _clients.size() + _clientsBuff.size();
	}

private:
	SOCKET _sock;
	std::vector<ClientSocket*> _clients;        //定义一个全局向量变量来存储正式客户队列socket  类型为啥是*  是因为不写*好像是在栈上创建的  加了*  子类使用都是用new来生成的  这样就在堆上创建了
	//缓冲客户队列
	std::vector<ClientSocket*> _clientsBuff;	//缓存客户端队列 
	std::mutex _mutex;							//锁
	std::thread _Thread;						//用来存  专门执行OnRun的线程对象
	INetEvent* _pNetEvent;						//声明一个网络事件类接口变量
public:
		//std::atomic_int _recvCount;				//声明一个原子整数
	std::atomic_long  _recvCount;			////声明一个原子长整数



	
};//class CellServer	

//-----------------------------------------------------------------------------------------------------------------------------------------------

 class EasyTcpServer :public INetEvent
{
private:
	SOCKET _sock;
	//std::vector<ClientSocket*> _clients;        //定义一个全局向量变量来存储客户端的socket  类型为啥是*  是因为不写*好像是在栈上创建的  加了*  子类使用都是用new来生成的  这样就在堆上创建了
	//消息处理对象,内部会创建线程
	std::vector<CellServer*> _cellServers;		
	//每秒消息计时
	CELLTimestamp _tTime;						
	//接收的消息计数
	std:: atomic_int _recvCount;	
	//客户端计数
	std::atomic_int _ClientCount;

	
public:	
	EasyTcpServer() {
		_sock = INVALID_SOCKET;//声明一个无效的socket  
		_recvCount = 0;
		_ClientCount = 0;

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

		return ret;	//1在其他语音就是true的意思
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
		SOCKET cSock = INVALID_SOCKET;        //定义一个无效的socket地址
#ifdef _WIN32
		cSock = accept((int)_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
		cSock = accept((int)_sock, (sockaddr*)&clientAddr, (socklen_t *)&nAddrLen);
#endif
		if (INVALID_SOCKET == cSock)
		{
			//进来表示接收到的socket的连接是无效的
			printf("Socket=<%d>错误,接收到无效客户端socket。\n", (int)_sock);
		}
		else
		{
			//将新客户端分配给客户数量最少的CellServer
			addClientToCellServer(new ClientSocket(cSock));
			//(inet_ntoa)(clientAddr.sin_addr)   获取ip地址
			//不回复 是因为 不知道是否是我们的正规流程的客户端
			//printf("socket=<%d>新客户端<%d>加入:, IP=%s \n", (int)cSock,_clients.size(),  (inet_ntoa)(clientAddr.sin_addr));//(inet_ntoa)表示把IP地址转换成可读性的字符串了
		}
		return cSock;
			
	}//SOCKET Accept()

	//增加客户端到小型服务
	void addClientToCellServer(ClientSocket* pClient)
	{
		//把客户端的放入ClientSocket*类型的向量里面
		//_clients.push_back(pClient);
		//查找客户数量最少的CellServer消息处理对象
		auto pMinServer = _cellServers[0];
		//迭代  找出连接客户端数量最少的
		for (auto pCellServer : _cellServers)
		{
			if (pMinServer->getClientCount() > pCellServer->getClientCount())
			{
				pMinServer = pCellServer;
			}
		}
		//把加入的客户端添加到这个小型服务里面
		pMinServer->addClient(pClient);
		OnNetJoin(pClient);
	}

	void Start(int nCellServer=1)
	{
		for (int n = 0; n < nCellServer; n++)
		{
			auto ser = new CellServer(_sock);
			_cellServers.push_back(ser);
			ser->setEventObj(this);
			ser->Start();
		}
	}

	//关闭Socket
	void Close() {

		if (INVALID_SOCKET != _sock)		//如果连接服务器的时候socket是有效的 我们初始化socket一下
		{

#ifdef _WIN32
			//8.关闭套接字socket  参数是要关闭哪个socket  这里是自己的
			closesocket(_sock);
			//清楚windows socket 环境
			WSACleanup();
#else

			//8.关闭套接字socket  参数是要关闭哪个socket  这里是自己的
			close(_sock->sockfd());

#endif

		}

	}// Close()

	
	//int _nCount = 0;
	//处理网络消息  已经搬家到CellServer  class里面了
	bool OnRun() {

		if (isRun())
		{
			time4msg();

			//伯克利 socket 描述符  windows 下面 第一个参数没有意义  虽然填写成了_sock+1 但是实际上在windows上不起作用
			fd_set fdRead;    //定义了一个描述符(socket)集合
			//fd_set fdWrite;
			//fd_set fdExp;
			//下面3个宏是用来fd_set清空
			FD_ZERO(&fdRead);    //清理集合
			//FD_ZERO(&fdWrite);
			//FD_ZERO(&fdExp);
			//下面的3个宏来设置对应socket对应的有没有可读  可写  异常的操作
			FD_SET(_sock, &fdRead);    //将_sock加入集合
			//FD_SET(_sock, &fdWrite);
			//FD_SET(_sock, &fdExp);

			timeval t = { 0,10 };        //select参数5类型是timeval  它是一个时间变量 有个成员变量 一个是秒  一个是微秒  不过windos的计时器好像还不能精确到微秒 所以我们用秒吧 {0,0,}表示不堵塞  不停的执行
										//nfds  是一个整数值  是指fd_set集合中所有的描述符(socket)的范围,而不是数量
										//参数1即是所有文件描述符最大值+1  在windows中这个参数无所谓 可以写0
			int ret = select(_sock + 1, &fdRead, /*&fdWrite*/0, /*&fdExp*/0,/* NULL*/&t);        //这个select一般来说最后一个数据填写NUll表示是阻塞模式 什么是阻塞模式咧就是当有客户端连接进来它才会有返回值  不然它就一直阻塞到这里 不给返回值  运行一下就知道  直接启动  客户端不连接 它会一直停在这里不往下走  如果不为null就是非堵塞模式了  填进去就表示多长时间去查询一下
			//printf("select ret=%d count=%d\n", ret, _nCount++);
			if (ret<0)
			{
				printf(" Accept   Select任务结束\n");
				Close();
				return false;
			}

			if (FD_ISSET(_sock, &fdRead))    //判断描述符(Socket)是否在集合中
			{
				FD_CLR(_sock, &fdRead);        //清理一下  这个清理很鸡贼只是清理了计数器 没有清理对应的数组
				Accept();							   // 4.accept 等待接受客户端连接  参数1还是服务端的socket  参数2是接收的客户端的socket  参数3表示客户端socket的长度
				return true;
			}
			return true;
		}//if (isRun())
		return false;
	}

	//是否工作中
	bool  isRun() {

		return _sock != INVALID_SOCKET;
	}
	//计算并输出每秒收到的网络消息 
	void time4msg() {

		//_recvCount++;
		auto t1 = _tTime.getElapsedSecond();
		if (t1 >= 1.0) {
	
			printf("thread<%d>,time<%lf>,socket<%d>,clients<%d>,_recvCount<%d>\n",_cellServers.size(), t1, _sock, (int)_ClientCount, (int)(_recvCount/t1));
			_recvCount = 0;
			_tTime.update();
		}
	}




	//发送数据给所有客户端
	/*void SendDataToAll(DataHeader* header)
	{
		for (int n = (int)_clients.size() - 1; n >= 0; n--)
		{
			SendData(_clients[n]->sockfd(), header);
		}
	}*/
	//只会被一个线程调用  
	virtual void OnNetJoin(ClientSocket* pClient)
	{
		_ClientCount++;
	}



	//虚函数OnLeave在EasyTcpServer类里面的实现
	//cellServer 4  多个线程触发 线程不安全  如果只开启了一个  就是安全的
	virtual void OnNetLeave(ClientSocket* pClient)
	{
		_ClientCount--;
	}
	//cellServer 4  多个线程触发 线程不安全   如果只开启了一个cellServer  就是安全的
	virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header)
	{
		_recvCount++;
	}
};

#endif // _EasyTcpServer_hpp_