#ifndef _MessageHeader_hpp_					//���û�ж���������δ��� 

#define  _MessageHeader_hpp_				//������  



enum   CMD
{
	CMD_LOGIN,                //��¼
	CMD_LOGIN_RESULT,        //��¼����
	CMD_LOGOUT,                //�ǳ�
	CMD_LOGOUT_RESULT,        //�ǳ�����
	CMD_NEW_USER_JOIN,        //���û�����
	CMD_ERROR                //��������

};

//��ͷ
struct DataHeader
{

	DataHeader()
	{
		dataLength = sizeof(DataHeader);
		cmd = CMD_ERROR;
	}
	short dataLength;        //���ݳ���
	short cmd;                //����


};

struct Login :public DataHeader        //д�ɼ̳�  Ȼ���ڽṹ��Ĺ��캯������д����ͷ������   �Ͳ��÷ֱ��Ͱ�ͷ�Ͱ����� ����һ������
{
	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];
	char data[932];/*1024-2-2-32-32*/		//��Ҫ�Ǵճ�1024�ֽ� 1kb��Сlogin

};
//DataPackage  ���շ��������ص����ݵİ���
struct LoginResult :public DataHeader
{
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
	char data[992]; /*1024-2-2-4*/		//��Ҫ�Ǵճ�1000�ֽ� �����= = ʵ��Ӧ����1024�ȽϺ�   1kb��СLoginResult
};

//int a = sizeof(Login);			//���ŵ�sizeof�� ���Կ�����ǰ������͵Ĵ�С   һ��Ҫ��4����  �ڴ��ֽڶ���


//�¿ͻ��˵����ݽṹ
struct NewUserJoin :public DataHeader
{
	NewUserJoin() {
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		socket = 0;
	}
	int socket;
};



//DataPackage  �ǳ����ݵİ���
struct Logout :public DataHeader
{
	Logout() {
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};
//DataPackage  ���������յ��ǳ��󷵻ص����ݵİ���
struct LogoutResult :public DataHeader
{
	LogoutResult() {
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

#endif  _MessageHeader_hpp_#pragma once
