
#include "../inc/mathTools.h"
float distance(const Point2f& a,const Point2f& b) {
	float distance=sqrt((a.x - b.x)* (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	return distance;
}
double calAngle(const Point2f& a,const Point2f& b) {
	float y = a.y - b.y;
	float x = a.x - b.x;
	float ratio = y / x;
	float atan1 = atan(ratio);
	return atan1/ CV_PI * 180;

}
double calAngle1(const Point2f& a, const  Point2f& b)
{
	float dot1 = a.x * b.x + a.y * b.y;
	float dot2 = distance(a, cv::Point2f(0, 0)) * distance(b, cv::Point2f(0, 0));
	return acos(dot1 / dot2) / CV_PI * 180;
}
