#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
using namespace std;
using namespace cv;

float distance(const Point2f&, const Point2f&);
double calAngle(const Point2f&, const Point2f&);
double calAngle1(const cv::Point2f& a, const cv::Point2f& b);
