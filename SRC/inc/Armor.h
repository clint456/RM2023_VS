#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;


struct Parm 
{
	float  light_angle;
	float  lights_angle;
	float lights_length_ratio;
	float  armor_angle;
	float x_ratio;
    float min_gapgate;
    float max_gapgate;
	Parm() 
	{
		light_angle = 15;//MAX  15    (deng tiao jiao du)
		lights_angle = 20;//MAX 5   //6 OK no bigger   20  //15  (deng tiao jiao du chao)
		armor_angle = 15;//MAX 7   10   (zhuang jia ban qin xie jiao)
		lights_length_ratio = 1.5;//MAX 1.5  //5  OK no problems big maybe (chang du bi)
		x_ratio =  3;//MAX 2.849
		min_gapgate=1.7;   //1.9   (zhuang jia ban chang kuan bi)
		max_gapgate=2.8;	//4.0
	}
};
enum  Enemy 
{
	 RED = 0,
	 BLUE = 1
} ;
class lightBar 
{
public:
	//lightBar();
	
	
	float angle;
    float height;
	float width;
	Point2f top, down;
	Point2f center;
	RotatedRect rect;
	RotatedRect elli_rect;
	int color;
};
class armorBox 
{
public:
	//armorBox();
    armorBox(const lightBar& , const lightBar&);

    armorBox();
	//pnp eigen;//TODO::targetArmor can have own struct
    lightBar l_light, r_light;
	Point2f center;
    Point2f angle;//x:pitch  y:yaw
	Point2f ver[4];
    float distance{};
    int index{};
	float lights_angle{};
	float length_ratio{};
	float horizon_angle{};
	float xDiff_ratio{};
    float gapgate{};
    float rt_angle{};
    float lt_angle{};
	
};
class armorDetector 
{
public:
	int roi_cnt;
	
	Rect roi_rect;
	bool isRoi;
	Mat dst;//zuihou chuli de pic
	Mat mat;//camera matrix
	Mat srcImage;//input image
	Mat src_bin;//threshold
	vector<lightBar> lights;
	vector<armorBox> armors;
	vector<vector<Point>> contours;
	Enemy enemy;
	armorBox targetArmor;
	Point2f targetPoint;//其实就是目标装甲板的center
	Point2f last_targetPoint;//上一帧的目标装甲板位置
	Point2f real_targetPoint;
	Point2f kf_Point;//把targetPointkalmen后过的点
	float yaw;float pitch;//计算出来的pitch,yaw
	int  max_light_angle = 1;
	int  max_lights_angle = 1;
	float max_lights_length_ratio = 1;
	int  max_armor_angle = 1;
	float max_x_ratio = 1;
public:
    armorDetector();
	armorDetector(string );
	void run(cv::Mat&);
	void setImage(cv::Mat );
	void findLights();
	void matchArmors();
	void drawArmors(const armorBox&);
	static void printData(const armorBox&);
	void adjustlightBarRect(cv::RotatedRect&, lightBar&);
	void adjustlightBarAngle(lightBar&,cv:: RotatedRect&);
	void judgelightBarColor(lightBar&, cv::Mat&);
	static bool isSuitableArmor(armorBox&);
	Rect addRoi(Rect&);//上一帧识别到了，现在没有识别到的话，扩大ROI区域，在代码开始
	Rect getRoi(armorBox&,Rect);//识别到装甲板之后，找出ROI区域，在代码结束部分
	void setNumber();
	void clearVector();
	static void getDistance(armorBox &);
	void putVer(armorBox&);
	void getAngle(armorBox &);
	void getAngle(Point2f&);
	void getAngle(Point2f& ,Point2f &);
	void selectArmor(vector<armorBox> );
};
class Predictor 
{
public:
    Point2f top_aim_point;
    Point2f predict_point;
    float predict_v_x;
    float predict_v_y;
    armorBox change_armor;
    armorBox armor;
    float distance;
    int choice;//�0�6�0�4�0�1�0�4�0�6�0�1�0�8���0�8�0�2�0�8��0�5�0�9�0�4�0�8�0�2armor�0�1�0�3�0�7�0�4���0�3�0�6�0�6�0�0�0�8�0�5�0�1�0�2�0�5�0�1�0�5�0�9�0�9�0�6�0�2�0�0�0�8�0�0�0�1�0�2�0�91�0�5�0�1�0�4�0�3�0�6�0�5�0�2�0�5�0�1�0�3�0�6�0�3armor�0�6�0�6�0�0�0�8�0�8�0�2�0�5�0�2���0�5�0�1
    //�0�6�0�4�0�1�0�4���0�3�0�6�0�6�0�0�0�8�0�5�0�1�0�2�0�5�0�1�0�5�0�9�0�9�0�6�0�2�0�0�0�8�0�0�0�1�0�8�0�50�0�5�0�1�0�5�0�3�0�7�0�1�0�4�0�3�0�6�0�5�0�0�0�1�0�4�0�0�0�6�0�5�0�0�0�4�0�8�0�2�0�5�0�2���0�3�0�5
    float times;
    float bug_time1,bug_time2;
    Predictor();
    //bool judge_top();


};
