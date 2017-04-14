#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "color.h"

#ifndef MOTION_H
#define MOTION_H

using namespace cv;
using namespace std;

class Motion{
private:
	Mat fgMaskMOG;
	Ptr<BackgroundSubtractor> pMOG;
public:
	double learningrate = 0.5;
	//default capture width and height
	int FRAME_WIDTH = 640;
	int FRAME_HEIGHT = 480;
	//max number of objects to be detected in frame
	int MAX_NUM_OBJECTS = 30;
	//minimum and maximum object area
	int MIN_OBJECT_AREA = 50;//10 * 20;
	int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
	int AVERAGE_SIZE = 10;
	string window_name = "Motion Tracking";
	vector<Point> targets;
	Morph_size morph_size;

	void iniMotionTracking();
	void mainMotionTracking(Mat *frame);
};

void sortTargets(vector<Point> targets);

#endif