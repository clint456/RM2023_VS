  
#include "../inc/mathTools.h"
#include "../inc/Armor.h"
//#define TEST
Parm parm;

// armorDetector::armorDetector() 
// {
// }

armorDetector::armorDetector(string matrix) 
{
    cv::FileStorage file(matrix,cv::FileStorage::READ);
    //if (file.isOpened()) 
    //{
        // file["Matrix"] >> mat;
        mat = (Mat_<double>(3,3)<< 
1.032705715359873e+03,0,320,
0,1.029836994504684e+03,240,
3.178832185092280e+02,2.673548277123335e+02,1);
    //}
    //circle(srcImage,Point(mat.at<double>(0,2),mat.at<double>(1, 2)),4,Scalar(128,0,120),4);
    // data: [ 1.4137187088093697e+03, 0., 6.6167485552542621e+02, 
    //        0.,1.4114887861673517e+03, 5.0865060122369590e+02, 
    //        0., 0., 1. ]
}
void armorDetector::adjustlightBarRect(RotatedRect& rect,lightBar& light) 
{
	light.height = MAX(rect.size.height, rect.size.width);
	light.width = MIN(rect.size.height, rect.size.width);

    //Point2f vertex[4];

    /*if(rect.size.height>rect.size.width){
        light.down=(vertex[0]+vertex[3])/2;
        light.top=(vertex[1]+vertex[2])/2;
    }                                                                                                                                                                                                                                                                                                        
    else{
        light.down=(vertex[2]+vertex[3])/2;
        light.top=(vertex[1]+vertex[0])/2;
    }*/
}
void armorDetector::adjustlightBarAngle(lightBar &light,RotatedRect &elli_Rect) {
    Point2f vertex[4];
    light.center = elli_Rect.center;
	light.angle = elli_Rect.angle;
        //light.center = elli_Rect.center;
    if (light.angle > 90) {
        light.angle = light.angle - 180;
        elli_Rect.points(vertex);
        light.top = (vertex[0] + vertex[3]) / 2;
        light.down = (vertex[1] + vertex[2]) / 2;
    }
    else {
        elli_Rect.points(vertex);
        light.top = (vertex[1] + vertex[2]) / 2;
        light.down = (vertex[0] + vertex[3]) / 2;
    }
}

void armorDetector::judgelightBarColor(lightBar& light, Mat& src) 
{
    int red = 0;int blue =0; 
    int area;
	auto region = light.rect.boundingRect();
	region.x -= fmax(3, region.width * 0.1);
	region.y -= fmax(3, region.height * 0.05);
	region.width += 2 * fmax(3, region.width * 0.1);
	region.height += 2 * fmax(3, region.height * 0.05);
	region &= Rect(0, 0, src.cols, src.rows);
	Mat roi = src(region);
        area=roi.cols*roi.rows;
	int red_cnt = 0, blue_cnt = 0;
	//imshow("roi",roi);
	for (int row = 0; row < roi.rows; row++) 
    {
		for (int col = 0; col < roi.cols; col++) 
        {
			red_cnt = roi.at<Vec3b>(row, col)[2];
			blue_cnt = roi.at<Vec3b>(row, col)[0];
            //printf("red_cnt:  %d    blue_cnt:  %d\n",red_cnt,blue_cnt);
            if(red_cnt-blue_cnt>10){
                red++;
            }
            if(blue_cnt-red_cnt>10){
                blue++;
            }
		}
	}
    if(red>blue){
        light.color=RED;
    }
    if(blue>red){
        light.color=BLUE;
    }
	/*if (red > blue&&(float)red/(float)area>0.6) {
        printf("red   red:  %d     blue:  %d\n",red,blue);
        printf("rate   :%f\n",(float)red/(float)area);
        light.color = RED;//0
	}
	if(blue>red&&(float)blue/(float)area>0.6) {
        
        printf("blue  red:  %d     blue:  %d\n",red,blue);
        printf("rate   :%f\n",(float)blue/(float)area);
		light.color = BLUE;//1
	}
    printf("----------------------------------\n");*/
}

//----------------------------------------------------------------------------------------------------------
void armorDetector::setImage(Mat src) 
{
//cout<<"roi center:  "<<roi.second.x+0.5*roi.second.width<<" , "<<roi.second.y+0.5*roi.second.height<<endl;
	Mat draw;
	vector<Vec4i> hierarchy;
	src.copyTo(srcImage);
	//Mat dst;
	//src.copyTo(dst);
	if(isRoi)
    {
		dst=src(roi_rect);
		//printf("scan roi\n");
	}
	else
    {
		src.copyTo(dst);
		//printf("scan all pic\n");
	}
	//cout<<roi_rect<<endl;
	//imshow("dst",dst);
//--------------------------------------------------------------------------------------------------------

//judge red and blue     
	Mat outpic;     	
	cvtColor(dst,outpic, 10);                       //gray picture
    int total_pixel = dst.rows * dst.cols;                  //������*�����ظ�ֵ��������total_pixelֵ
	const uchar * ptr_src = dst.data;                       //����ָ��ͼ�����src���ص���ʼλ�õ�ָ��ptr_src
	uchar *ptr_dst = outpic.data;                           //����ָ��ͼ�����color_thr���ص���ʼλ�õ�ָ��ptr_dst
	const uchar *ptr_dst_end = outpic.data + total_pixel;   //����ָ��ͼ�����color_thr���ص���ֹλ�õ�ָ��ptr_dst_end
	uchar b, r,g;
	if (enemy == BLUE) 
    {                       //�з�Ϊ����ʱ
		for (; ptr_dst != ptr_dst_end; ptr_src += 1, ptr_dst += 1) {
			b = *ptr_src;   //ͼ�����src�����ص�bֵ��ֵ��b
			ptr_src += 1;   //ָ��ptr_srcָ�����2
			g = *ptr_src;   //ͼ�����src�����ص�rֵ��ֵ��r
            ptr_src += 1;
            r = *ptr_src;
            if(g >= 60)
            {
                *ptr_dst = 0;
            }
			if (b - r > 60/*color_thr_value*/) 
            {//30
				*ptr_dst = 255;
			}//���b - r ��ֵ����color_thr_value,ͼ�����color_thr�����ص���ɫתΪ��ɫ
			else 
            {
				*ptr_dst = 0;
			}//����ͼ�����color_thr�����ص���ɫתΪ��ɫ
		}
	}
	if (enemy == RED) {//�з�Ϊ�췽ʱ
		for (; ptr_dst != ptr_dst_end; ptr_src += 1, ptr_dst += 1) {
			b = *ptr_src;//ͼ�����src�����ص�bֵ��ֵ��b
			ptr_src +=1;//ָ��ptr_srcָ�����2
			g = *ptr_src;//ͼ�����src�����ص�rֵ��ֵ��r
            ptr_src +=1;
            r = *ptr_src;
            if(g >= 100)
            {
                *ptr_dst = 0;
            }
			if (r - b >100 /*color_thr_value*/) {//120
				*ptr_dst = 255;
			}//���r - b ��ֵ����color_thr_value,ͼ�����color_thr�����ص���ɫתΪ��ɫ
			else {
				*ptr_dst = 0;
			}//����ͼ�����color_thr�����ص���ɫתΪ��ɫ
		}
	
	}
    erode(outpic,outpic, getStructuringElement(MORPH_RECT,Size(3,3)));
    morphologyEx(outpic,outpic,MORPH_CLOSE,getStructuringElement(MORPH_RECT,Size(3,3)));
    dilate(outpic,outpic, getStructuringElement(MORPH_RECT,Size(3,3)));
    //namedWindow("have rb",WINDOW_NORMAL);
    imshow("have rb",outpic);
    //findContours(outpic, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

//no have rb----------------------------------------------------------------------------------------
    Mat gray;//cvtcolor
    Mat thre;//not have rb
    cvtColor(dst,gray,10);                                              //gray picture
    threshold(gray,thre,60,255,THRESH_BINARY);                          // binaried picture
    dilate(thre,thre, getStructuringElement(MORPH_RECT,Size(3,3)));     //swell
    //namedWindow("no rb",WINDOW_NORMAL);
    imshow("no rb",thre);                                               //put down speed
//---------------------------------------------------------------------------------------------------

    Mat best(src.rows,src.cols,CV_8UC1);                                //one channel picture
    bitwise_and(outpic,thre,best);                                      // &
    dilate(best,best, getStructuringElement(MORPH_RECT,Size(3,3)));     //swell
    //namedWindow("best",WINDOW_NORMAL);
    imshow("best",best);                                                //put down speed

    findContours(best, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE); //find board

//------------------------------------------------------------------------------------------------

//old
	// threshold(outpic, src_bin, 100, 255, THRESH_BINARY);
    // //blur(src_bin,src_bin,Size(1,1));
    // dilate(src_bin,src_bin, getStructuringElement(MORPH_RECT,Size(3,3)));
    // //imshow("two",src_bin);
    // imshow("src_bin",src_bin);
	//findContours(src_bin, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    //printf("contours size %d\n",contours.size());
//-------------------------------------------------------------------------------------------
	auto it = contours.begin();
    	//draw= Mat::zeros(srcImage.size(), srcImage.type());
	for (; it < contours.end(); ) 
    {
		//cout << (*it).size() << endl;
		if ((*it).size() < 13  ||(*it).size() > 1000)       
        {//no problems   big maybe
            //after 10m,contour<10
            printf("           contour size:   %d\n",(*it).size());
			it = contours.erase(it);

		}
		else 
        {
			it++;
		}
	}
	// for (it = contours.begin(); it < contours.end();) {
	// 	lightBar light;
	// 	light.elli_rect = fitEllipse((*it));
	// 	if (light.rect.size.area() > 600) {
	// 		it = contours.erase(it);
	// 	}
	// 	else{
	// 	    it++;
	// 	}
	// }
    //drawContours(draw,contours,-1,Scalar(0,255,255),2);
    //imshow("draw",draw);
}

void armorDetector::findLights() 
{
	vector<vector<Point>>::iterator it;
   //printf("contours        :%d\n",contours.size());
	for (it = contours.begin(); it < contours.end(); it++) 
    {
			lightBar light;
			light.rect = minAreaRect((*it));
			light.elli_rect = fitEllipse((*it));
            adjustlightBarRect(light.rect,light);
			adjustlightBarAngle(light, light.elli_rect);

            if (light.height / light.width < 1.0)//no prombles  big maybe
			{
                printf(" 3.3:   %f\n",light.height / light.width);
				continue;
			}
			if (abs(light.angle) < parm.light_angle)
			{
                // printf(" 4.4:    %f\n",light.angle);
                lights.push_back(light);
				// judgelightBarColor(light,dst);
				// if (light.color == enemy) {
				// 	
				// }
			}

			

		}
    sort(lights.begin(), lights.end(),
				[](lightBar& a1, lightBar& a2) {
					return a1.center.x < a2.center.x; });
    for(int i=0;i<lights.size();i++){
        line(srcImage,lights[i].top,lights[i].down,Scalar(0,255,0),6);
    }
	//imshow("dst",dst);
    imshow("srcImage",srcImage);
        //printf("1.1     %d\n",contours.size());
        //for(int i=0;i<lights.size();i++){
        //   printf(" %d angle  :  %f\n",i,lights[i].angle);
        //    //putText(srcImage,to_string(i),lights[i].center,1,1,Scalar(255,255,255),2);
        //}
        //imshow("srcImage",srcImage);
}
void armorDetector::matchArmors() {
	for (int i = 0; i < lights.size() - 1; i++) {
		for (int j = i + 1; j < lights.size(); j++) {
			armorBox armor = armorBox(lights[i], lights[j]);
			if (isSuitableArmor(armor)) {
				armors.push_back(armor);
			}
		}
	}
	//if (!armors.empty()) {
		//setNumber();
	//}judgelightBarColor
}
void armorDetector::setNumber() {
	int n = 0;
	for (; n <= armors.size()-1; n++) {
		armors[n].index = n;
		putText(srcImage, to_string(n), armors[n].center, FONT_HERSHEY_PLAIN, 3, Scalar(255, 192, 203), 4, 8 );
	}
}
bool armorDetector::isSuitableArmor( armorBox &armor) {
	armor.lights_angle = abs(armor.l_light.angle - armor.r_light.angle);
	armor.length_ratio = armor.l_light.height / armor.r_light.height;
	armor.length_ratio = ((armor.length_ratio > 1.0) ? armor.length_ratio : (1.0 / armor.length_ratio));
	armor.horizon_angle = abs(calAngle(armor.l_light.center, armor.r_light.center));
    float px_angle = abs(calAngle1(armor.l_light.top-armor.r_light.top, armor.l_light.down-armor.r_light.down));
   // printf(" 0.0   %f\n",armor.horizon_angle);
	float meanLen = (armor.l_light.height + armor.r_light.height) / 2;
	float xDiff = abs(armor.l_light.center.x - armor.r_light.center.x);  
	armor.xDiff_ratio = xDiff / meanLen;
    armor. lt_angle = abs(calAngle1(armor.l_light.top - armor.l_light.down, armor.l_light.top - armor.r_light.top));
	armor. rt_angle = abs(calAngle1(armor.l_light.top - armor.r_light.top, armor.r_light.top - armor.r_light.down));
	armor.gapgate=distance(armor.l_light.center,armor.r_light.center)/min(armor.l_light.height,armor.r_light.height);
    if(armor.l_light.top.x<0||armor.l_light.top.y<0||armor.l_light.down.x<0||armor.l_light.down.y<0||armor.r_light.top.x<0||armor.r_light.top.y<0||armor.r_light.down.x<0||armor.r_light.down.y<0)
    {
        return 0;
    }
    if (armor.lights_angle > parm.lights_angle)  //go to Armor.h 2
    {
		//printf("1111    lights_angle:  %f\n",armor.lights_angle);
		return 0;
	}
    else if (armor.horizon_angle > parm.armor_angle) 
    {
        //printf("2222   :horizon_armor_angle:  %f\n",armor.horizon_angle);
        return 0;
    }
    //else if(px_angle>10){
    //    
    //    return 0;
    //}
    
	else if(armor.length_ratio > parm.lights_length_ratio){
        //printf("3333   :length_ratio:  %f\n",armor.length_ratio);
		return 0;
	}

	// else if (armor.xDiff_ratio > parm.x_ratio) {
    //     printf("4444    :xDiff_ratio:    %f\n",armor.xDiff_ratio);
	// 	return 0;
	// }
    else if(armor.gapgate<parm.min_gapgate||armor.gapgate>parm.max_gapgate){
        //printf("5555    :gapgate:        %f\n",armor.gapgate);
        return 0;
    }
	/*else if (armor.rt_angle > 105 ||armor. rt_angle < 75) {
		//printf ("rt_angle:    %f\n",armor.rt_angle );
		return 0;
	}
	else if (armor.lt_angle > 105 ||armor. lt_angle < 75) {
		//printf ("rt_angle:    %f\n",armor.rt_angle );
		return 0;
	}*/
    // else if(armor.l_light.rect.size.area()/armor.r_light.rect.size.area() >2 || armor.l_light.rect.size.area()/armor.r_light.rect.size.area() <0.5 )
    //     return 0;
	else {
		return 1;
	}

}
void armorDetector::getDistance(armorBox &a){
    cv::Point2f topL = a.l_light.top;
    cv::Point2f topR = a.l_light.top;
    cv::Point2f downL = a.r_light.down;
    cv::Point2f downR = a.r_light.down;
    float height = distance(topL, downL)*0.5 + distance(topR, downR)*0.5;
    ushort predictDistance=0;
    predictDistance = (ushort)( 15500/ height);//123400    multiplying power  change by fact
    //predictDistance=(ushort)(9.77*exp(-0.09967*height)+2.698*exp(-0.01489*height));
    //printf(" %f\n",predictDistance);
    a.distance = predictDistance;
    //printf("%f",a.distance);
} 


void armorDetector::getAngle(armorBox &a) {
    double x_y=a.center.x- mat.at<double>(0,2);
    double y_y=a.center.y - mat.at<double>(1, 2);
    double x_x=mat.at<double>(0, 0);//x轴的焦距
    double y_x=mat.at<double>(1, 1);//y轴的焦距
    imshow("srcImage",srcImage);
    a.angle.x = atan2(x_y, x_x )* 180.0 / CV_PI;
    a.angle.y = atan2(y_y, y_x )* 180.0 / CV_PI;
 }


// void armorDetector::getAngle(Point2f &center) {
//     double x_y=center.x - mat.at<double>(0, 2);
//     double x_x=mat.at<double>(0, 0);//x轴的焦距
//     double y_y=center.y - mat.at<double>(1, 2);
//     double y_x=mat.at<double>(1, 1);//y轴的焦距
//     //circle(srcImage,Point(mat.at<double>(0,2),mat.at<double>(1, 1)),4,Scalar(128,0,120),4);
//     //imshow("srcImage",srcImage);
//     yaw = atan2(x_y, x_x )* 180.0 / CV_PI;
//     pitch = atan2(y_y, y_x )* 180.0 / CV_PI;
    

// }
// void armorDetector::getAngle(Point2f &a,Point2f &b) {
//     double x_y=a.x - mat.at<double>(0, 2);
//     double x_x=mat.at<double>(0, 0);
//     double y_y=a.y - mat.at<double>(1, 2);
//     double y_x=mat.at<double>(1, 1);
//     b.x = atan2(x_y, x_x )* 180.0 / CV_PI;
//     b.y = atan2(y_y, y_x )* 180.0 / CV_PI;
    
// }
void armorDetector::drawArmors(const armorBox& armor) {
	line(srcImage, armor.l_light.top, armor.r_light.top, Scalar(0, 255,0 ), 1);
	line(srcImage, armor.l_light.top, armor.l_light.down, Scalar(0, 255,0 ), 1);
	line(srcImage, armor.l_light.down, armor.r_light.down, Scalar(0, 255,0 ), 1);
	line(srcImage, armor.r_light.top, armor.r_light.down, Scalar(0, 255,0 ), 1);
	//imshow("srcImage", srcImage);
}
void armorDetector::printData(const armorBox& armor) {
	//5     3
	printf("  %d  \n", armor.index);
	printf("       %f     :   %f     <%f\n", armor.l_light.angle, armor.r_light.angle,parm.light_angle);
	printf("        %f     <%f\n", armor.lights_angle,parm.lights_angle);

    printf("     :   %f     <%f\n", armor.horizon_angle,parm.armor_angle);
    printf("     :   %f     <%f\n", armor.length_ratio,parm.lights_length_ratio);
	printf("     :   %f     <%f\n", armor.xDiff_ratio,parm.x_ratio);
    printf("     :   %f     <70||>110\n", armor.rt_angle);
    printf("\n");
}
void armorDetector::clearVector() {
    if(!lights.empty()){
        vector<lightBar>().swap(lights);
    }
    if(!armors.empty()){
        vector<armorBox>().swap(armors);
    }
    if(!contours.empty()){
        vector<vector<Point>>().swap(contours);
    }
}
void armorDetector::run(Mat& src) {

	setImage(src);
	
    findLights();
   // cout<<"lights:  "<<lights.size()<<endl;

	if (lights.size() < 2) {    //  if the number of lights < 2
		isRoi=false;     //sign
	  	roi_rect=Rect();
        	clearVector(); 
       
        	return;
	}
	matchArmors();

	 if (armors.empty()) {
		isRoi=false;
	  	roi_rect=Rect();
       		clearVector();
        	return;
	}
    // --------------------------------------------------------------------

	for (auto & armor : armors) {
	    drawArmors(armor);
		//printData(armor);
	 	getDistance(armor);
	    //getAngle(armor);
	}
    	selectArmor(armors);  
	    getAngle(targetArmor);
	    drawArmors(targetArmor);


}
Rect armorDetector::getRoi(armorBox& armor,Rect roi_rect1){
    int width,height;
    width=(armor.r_light.top.x-armor.l_light.top.x)*0.5+(armor.r_light.down.x-armor.l_light.down.x)*0.5;
    height=(armor.l_light.down.y-armor.l_light.top.y)*0.5+(armor.r_light.down.y-armor.r_light.top.y)*0.5;
    width*=3;
    height*=3;
    Rect roi;

    roi.x=armor.center.x-width;
    roi.y=armor.center.y-height;
    roi.width=width*2;
    roi.height=height*2;
    if(roi.x<0){roi.x=0;}
    if(roi.y<0){roi.y=0;}
    if(roi.width+roi.x>=480){roi.width=640-roi.x;}
    if(roi.height+roi.y>=480){roi.height=480-roi.y;}

    return roi;
    
}
void armorDetector::selectArmor(vector<armorBox> armors) {
    ushort  minDistance = 65535;
    minDistance = armors[0].distance;
    targetArmor = armors[0];
    for (armorBox &a : armors) {
        if (a.distance == 0) {
            continue;
        }
        if (a.distance < minDistance) {
            targetArmor = a;
            minDistance = a.distance;
        }
    }
    

    
}

