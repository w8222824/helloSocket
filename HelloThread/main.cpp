#include<iostream>
#include<thread>	//c++11�Ŀ�ƽ̨�߳̿�
#include<mutex>		//��
#include <atomic>	//ԭ��
#include "CELLTimestamp.hpp"	//c++11ʱ����

using namespace std;

//ԭ�Ӳ���    ԭ��   ����

mutex  m;		//ȫ����
const int tCount = 4;	//ȫ���߳�����
atomic_int	sum = 0;	//
//int  sum = 0;
void workFun(int index) {


	for (size_t i = 0; i < 20000000; i++)
	{
		//lock_guard<mutex>  lg(m);			//�Խ���   ���Ƿ�װ��һ���lock �� unlock  ��Ȼ����ʱ�䳤��һ�� ���ǲ��õ���������ת��ȥ��
		//m.lock();	//��ס ����ִ����  ����ִ����������
		sum++;
		//cout << index << "Hello,other thread."  << i<< "\n" <<endl;
		//m.unlock();
	}

}//��ռʽ  CPU����Դ


int main() {
	//thread t(workFun,10);		//��������һ���߳�  
	thread t[tCount];
	for (size_t i = 0; i < tCount; i++)
	{
		t[i] = thread(workFun,i);
		//t[i].detach();	//���̺߳������̷߳���  �������߳�  �������̺߳������߳����첽��
		//t[i].join();
	}
	CELLTimestamp tTime;
	for (size_t i = 0; i < tCount; i++)
	{
		
		//t[i].detach();	//���̺߳������̷߳���  �������߳�  �������̺߳������߳����첽��
		t[i].join();		//Join�����̻߳�ִ����t�߳���������н����Ż��������������
	}
	
	
	//t.join();	//Join�����̻߳�ִ����t�߳���������н����Ż��������������
	cout << tTime.getElapsedTimeInMilliSec() << "  ,sum=" << sum << endl;
		cout << "Hello,main thread.\n" << endl;
		sum = 0;
		tTime.update();
		
		for (size_t i = 0; i < 80000000; i++)
		{
			sum++;
		}

		cout << tTime.getElapsedTimeInMilliSec()<< "  ,sum2=" << sum << endl;


	system("PAUSE");
	return 0;

	
}