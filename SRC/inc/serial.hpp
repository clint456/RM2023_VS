#pragma once

#include <stdio.h>
#include <string.h>

#include <fcntl.h> 
#include <errno.h>
#include <atomic>
#include <iostream>
#include <opencv2/opencv.hpp>



using namespace std;

class Uart{
public:
	Uart();
	~Uart();
	bool Open(const char * device, int _speed, int _parity, bool _should_block,int data_length);
	bool restart();
		
	//bool ReadArmor(unsigned char data[10],int &num);
	//TODO 主控发送时小数乘以10变为int，最后乘以10 
	bool ReadMode(unsigned char data[16],unsigned char &id,unsigned char &mode);
	bool ReadData(unsigned char data[16],unsigned char &mode,atomic<int>& pitch_now,atomic<int>& yaw_now,atomic<int>& shoot_vel);
	bool TestSend(int a);
	bool TestR(unsigned char a[]);
	
	//bool sendToMCUFinal(unsigned char fire,int target_pitch,int targrt_yaw,int distance);
	
	bool sendToMCU(unsigned char fire,short target_pitch,short target_yaw,short distance);
	//bool SendData(char* data,int len);
	bool SendData(unsigned char* data,int len) ;
	
	bool readId(unsigned char data[1],unsigned char &id);
	//加入刚开始获得的机器人信息（红蓝方判断）
		//bool Send( float yaw, short pitch, bool shoot, float timestamp, Mode mode);
	//bool InitStartValue(short & value);
	void Close();
	bool SendError();
	bool SendNoRobotNum();
private:
	const char *uart_path;
	int fd;
	int speed;
	int parity;
	bool should_block;
public:
	unsigned char mode;//瞄准模式
	float pitch_now;
	float yaw_now;
	float shoot_vel;
public:
	 char fire;
	 float target_pitch;
	 float targrt_yaw;
	 float distance;
public:
	 unsigned char id;

public:
	int success_=0;
	int fail_=0;
	int ze=0;
};


