#include "camera.hpp"


void Camera::cameraInit()
{

    if (cameraType == USBDevice)
    {
        cap.open(deviceID, apiID);
        if (!cap.isOpened()) {
            cerr << "ERROR! Unable to open camera\n";
        }
        //--- GRAB AND WRITE LOOP
        cout << "Start grabbing" << endl
            << "Press any key to terminate" << endl;
    }
    else if (cameraType == MDDevice)
    {
        exit(0);
    }
    else
    {
        exit(1);
    }
}

void Camera::process(Mat &src)
{
    if (cameraType == USBDevice)
    {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(src);
    }
    else if (cameraType == MDDevice)
    {
        exit(0);
    }
    else
    {
        exit(1);
    }

    // check if we succeeded
    if (src.empty()) {
        cerr << "ERROR! blank frame grabbed\n";
        exit(1);
    }
}