#include "../inc/ThreadControl.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <stdio.h>
#include <iostream>
#include "../device/camera.hpp"
#define TEST_CAM
using namespace cv;


#ifdef TEST_CAM
void processer::frameLoop() 
{
    armorDetector detector;
    Camera camera;
    detector.enemy = RED;   //在这里设定[地方]enemy颜色
    camera.cameraType = USBDevice;
    camera.cameraInit();
   
	Mat src;
  
    for(;;)
    {
        camera.imgProcess(src);
       
        imshow("Live", src);
        if (waitKey(5) >= 0)
            break;

        //flip(src, src, -1);   //画面反转函数
       
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


