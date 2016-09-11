#include "opencv2/highgui/highgui.hpp"
#include "color.h"
#include <opencv\cv.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace cv;
using namespace std;

void Color::iniColorTracking(){
	createTrackbars();
	//namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	//namedWindow(window_name2, CV_WINDOW_AUTOSIZE);
}

void Color::on_trackbar(int, void*)
{}

string Color::intToString(int number){
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void Color::createTrackbars(){
	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);

	createTrackbar("H_MIN", trackbarWindowName, &(H_MIN), 256);// , on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &(H_MAX), 256);// , on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &(S_MIN), 256);//, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &(S_MAX), 256);//, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &(V_MIN), 256);//, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &(V_MAX), 256);//, on_trackbar);
}
void drawObject(int x, int y, Mat &frame){
	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
}

void morphOps(Mat &frame, Morph_size morph_size){
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(morph_size.ero_size, morph_size.ero_size)); //33
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(morph_size.dil_size, morph_size.dil_size)); //8,8

	erode(frame, frame, erodeElement);
	erode(frame, frame, erodeElement);

	dilate(frame, frame, dilateElement);
	dilate(frame, frame, dilateElement);
}

void Color::trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed){
	targets.clear();
	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				if (area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10 / area;
					y = moment.m01 / area;
					objectFound = true;
					refArea = area;
				}
				else objectFound = false;

			}
			//let user know you found an object
			if (objectFound == true){
				targets.push_back(Point(x,y));
			}
		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}

void Color::mainColorTracking(Mat *frame)
{
	bool trackObjects = true;
	bool useMorphOps = true;
	//matrix storage for HSV image
	Mat HSV;
	//matrix storage for binary threshold image
	Mat threshold;
	//x and y values for the location of the object
	int x = 0, y = 0;

	cvtColor(*frame, HSV, COLOR_BGR2HSV);

	inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold);

	if (useMorphOps)
		morphOps(threshold,morph_size);

	if (trackObjects)
		trackFilteredObject(x, y, threshold, *frame);

	//show frames 
	//imshow(window_name2, threshold);
	//imshow(window_name, *frame);
}
