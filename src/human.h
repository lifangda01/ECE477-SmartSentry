#include <iostream>
#include <opencv2/opencv.hpp>

#ifndef HUMAN_H
#define HUMAN_H

using namespace cv;
using namespace std;

class Human{
private:
	HOGDescriptor hog;
public:
	string window_name = "Human Detection";
	vector<Point> targets;
	void iniHumanTracking();
	void mainHumanTracking(Mat *frame);
};

#endif