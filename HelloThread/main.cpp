#include<iostream>
#include<thread>	//c++11的跨平台线程库
#include<mutex>		//锁
#include <atomic>	//原子
#include "CELLTimestamp.hpp"	//c++11时间类

using namespace std;

//原子操作    原子   分子

mutex  m;		//全局锁
const int tCount = 4;	//全局线程数量
atomic_int	sum = 0;	//
//int  sum = 0;
void workFun(int index) {


	for (size_t i = 0; i < 20000000; i++)
	{
		//lock_guard<mutex>  lg(m);			//自解锁   就是封装了一层的lock 和 unlock  虽然可能时间长了一点 但是不用担心意外跳转出去了
		//m.lock();	//锁住 下面执行完  不会执行其他操作
		sum++;
		//cout << index << "Hello,other thread."  << i<< "\n" <<endl;
		//m.unlock();
	}

}//抢占式  CPU的资源


int main() {
	//thread t(workFun,10);		//声明定义一个线程  
	thread t[tCount];
	for (size_t i = 0; i < tCount; i++)
	{
		t[i] = thread(workFun,i);
		//t[i].detach();	//主线程和其他线程分离  并启动线程  这样主线程和其他线程是异步的
		//t[i].join();
	}
	CELLTimestamp tTime;
	for (size_t i = 0; i < tCount; i++)
	{
		
		//t[i].detach();	//主线程和其他线程分离  并启动线程  这样主线程和其他线程是异步的
		t[i].join();		//Join启动线程会执行完t线程里面的所有结果后才会出来继续走下面
	}
	
	
	//t.join();	//Join启动线程会执行完t线程里面的所有结果后才会出来继续走下面
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