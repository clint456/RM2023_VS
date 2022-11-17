#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

typedef enum CameraType {
	USBDevice = 0,
	MDDevice,
};


class Camera
{
public:
	

	int deviceID = 0;             // 0 = open default camera
	int apiID = CAP_ANY;      // 0 = autodetect default API
	
	VideoCapture cap;
	CameraType cameraType;
	void cameraInit();
	void imgProcess(Mat &src);


private:

};
