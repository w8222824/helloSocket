#ifndef _CELLTimestamp_hpp_			//如果不存在
#define _CELLTimestamp_hpp_			//就创建

//#include <windows.h>
#include<chrono>					//标准库里面的一个时间库  已经非常精准的那种   C++11才有的
using namespace std::chrono;

class CELLTimestamp			//单位（CELL） 时间戳（Time stamp） 就是定义了一个时间戳类
{
public:
    CELLTimestamp()
    {
        //QueryPerformanceFrequency(&_frequency);
        //QueryPerformanceCounter(&_startCount);
		update();
    }
    ~CELLTimestamp()
    {}
	//更新当前时间
    void    update()
    {
        //QueryPerformanceCounter(&_startCount);
		_begin = high_resolution_clock::now();		//获取当前时间  精度是 纳秒
    }
    /**
    *   获取当前秒
    */
    double getElapsedSecond()
    {
        return  getElapsedTimeInMicroSec() * 0.000001;
    }
    /**
    *   获取毫秒
    */
    double getElapsedTimeInMilliSec()
    {
        return this->getElapsedTimeInMicroSec() * 0.001;
    }
    /**
    *   获取微妙
    */
    long long getElapsedTimeInMicroSec()
    {
		/*
        LARGE_INTEGER endCount;
        QueryPerformanceCounter(&endCount)

        double  startTimeInMicroSec =   _startCount.QuadPart * (1000000.0 / _frequency.QuadPart);
        double  endTimeInMicroSec   =   endCount.QuadPart * (1000000.0 / _frequency.QuadPart);

        return  endTimeInMicroSec - startTimeInMicroSec;
		*/
		//返回时间段（duration_cast）  类型是微秒级别的     算法（当前时间纳秒级别的-之前计算的当前时间 纳秒级别的） .count  表示返回时间   
		return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count();
    }
protected:			//保护级别只能这个类用的
    //LARGE_INTEGER   _frequency;
    //LARGE_INTEGER   _startCount;
	time_point<high_resolution_clock> _begin;			//定义一个时间点类型  high_resolution_clock（高精度时间戳） 精度为纳秒级别
};

#endif // !_CELLTimestamp_hpp_