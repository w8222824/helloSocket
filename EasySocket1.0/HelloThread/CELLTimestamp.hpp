#ifndef _CELLTimestamp_hpp_			//���������
#define _CELLTimestamp_hpp_			//�ʹ���

//#include <windows.h>
#include<chrono>					//��׼�������һ��ʱ���  �Ѿ��ǳ���׼������   C++11���е�
using namespace std::chrono;

class CELLTimestamp			//��λ��CELL�� ʱ�����Time stamp�� ���Ƕ�����һ��ʱ�����
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
	//���µ�ǰʱ��
    void    update()
    {
        //QueryPerformanceCounter(&_startCount);
		_begin = high_resolution_clock::now();		//��ȡ��ǰʱ��  ������ ����
    }
    /**
    *   ��ȡ��ǰ��
    */
    double getElapsedSecond()
    {
        return  getElapsedTimeInMicroSec() * 0.000001;
    }
    /**
    *   ��ȡ����
    */
    double getElapsedTimeInMilliSec()
    {
        return this->getElapsedTimeInMicroSec() * 0.001;
    }
    /**
    *   ��ȡ΢��
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
		//����ʱ��Σ�duration_cast��  ������΢�뼶���     �㷨����ǰʱ�����뼶���-֮ǰ����ĵ�ǰʱ�� ���뼶��ģ� .count  ��ʾ����ʱ��   
		return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count();
    }
protected:			//��������ֻ��������õ�
    //LARGE_INTEGER   _frequency;
    //LARGE_INTEGER   _startCount;
	time_point<high_resolution_clock> _begin;			//����һ��ʱ�������  high_resolution_clock���߾���ʱ����� ����Ϊ���뼶��
};

#endif // !_CELLTimestamp_hpp_