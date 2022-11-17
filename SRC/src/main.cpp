// new Armor1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "../inc/ThreadControl.h"
using namespace cv;
using namespace std;
//Uart Serial;
int main()
{
    processer process;
#ifdef TEST_serial

    thread t1(&processer::getUartData, &process);
    thread t2(&processer::frameLoop, &process);
    t1.join();
    t2.join();

#endif
#ifndef TEST_serial
    thread t2(&processer::frameLoop, &process);
    t2.join();

#endif
    return 0;
}


