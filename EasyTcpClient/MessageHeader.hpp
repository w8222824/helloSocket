#ifndef _MessageHeader_hpp_					//如果没有定义声成这段代码 

#define  _MessageHeader_hpp_				//就生成  



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

#endif  _MessageHeader_hpp_