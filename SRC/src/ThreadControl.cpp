#include "../inc/ThreadControl.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <stdio.h>
#include <iostream>
#define TEST_CAM
using namespace cv;
unsigned char           * g_pRgbBuffer;     //处理后数据缓存区

#ifdef TEST_CAM
void processer::frameLoop() 
{
    string cameraMatrixFilename="/home/gs/GS/newArmor2/intrinsics_large.yml";
    armorDetector detector(cameraMatrixFilename);

    detector.enemy = RED;   //在这里设定[地方]enemy颜色
    
	Mat src;
    VideoCapture cap;

    int deviceID = 0;             // 0 = open default camera
    int apiID = CAP_ANY;      // 0 = autodetect default API
    cap.open(deviceID, apiID);
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
    }
    //--- GRAB AND WRITE LOOP
    cout << "Start grabbing" << endl
        << "Press any key to terminate" << endl;
    while (true) 
    {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(src);
        // check if we succeeded
        if (src.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        imshow("Live", src);
        if (waitKey(5) >= 0)
            break;
     
        //flip(src, src, -1);
       
        detector.run(src);
     
        circle(
            detector.srcImage,
            Point(detector.mat.at<double>(0, 2), detector.mat.at<double>(1, 2)),
            4, Scalar(0, 255, 0), 6);
        if (!detector.armors.empty()) {
          
            /*Serial.sendToMCU(1, (short)((detector.targetArmor.angle.y) * 100),
                           (short)(detector.targetArmor.angle.x * 100),
                           detector.targetArmor.distance);*/

          putText(detector.srcImage,
                  "distance   " + to_string(detector.targetArmor.distance),
                  Point(50, 250), 3, 1, Scalar(0, 0, 255), 2);
          putText(detector.srcImage,
                  "yaw:   " + to_string(detector.targetArmor.angle.x),
                  Point(50, 50), 1, 3, Scalar(0, 255, 0), 2);
          putText(detector.srcImage,
                  "pitch:   " + to_string(detector.targetArmor.angle.y),
                  Point(50, 150), 1, 3, Scalar(255, 0, 0), 2);

          circle(detector.srcImage, detector.targetArmor.center, 4,
                 Scalar(0, 0, 255), 3);
          imshow("srcImage", detector.srcImage);
        } else {
          
            /*Serial.sendToMCU(0, 0, 0, 0);*/
        }

        detector.clearVector();

        }
        imshow("srcImage",detector.srcImage);
    }
#endif
