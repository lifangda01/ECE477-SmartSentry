#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include "motion.h"
#include "color.h"
#include <stdio.h>
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;

void Motion::iniMotionTracking()
{
	//namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	pMOG = new BackgroundSubtractorMOG();
	morph_size.ero_size = 2;
	morph_size.dil_size = 15;
}

void Motion::mainMotionTracking(Mat *frame){

	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Rect objectBoundingRectangle;
	vector<Rect> rects,temp_rects;

	temp_rects.clear();
	rects.clear();
	targets.clear();
	pMOG->operator()(*frame, fgMaskMOG, learningrate);

	morphOps(fgMaskMOG, morph_size);

	findContours(fgMaskMOG, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

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
					objectFound = true;
					refArea = area;
					//drawContours(*frame, contours, index, Scalar(0,255, 0), CV_FILLED, 8, hierarchy);
					objectBoundingRectangle = boundingRect(contours[index]);
					temp_rects.push_back(objectBoundingRectangle);
					//targets.push_back(Point(objectBoundingRectangle.x + objectBoundingRectangle.width / 2, objectBoundingRectangle.y + objectBoundingRectangle.height / 2));
				}
				else objectFound = false;
			}
		}
	}

	//cluster smaller motion rectangles
	for (int i = 0; i < temp_rects.size(); i++)
	{
		rects.push_back(temp_rects[i]);
		rects.push_back(temp_rects[i]);
	}
	groupRectangles(rects, 1, 2);
	for (int i = 0; i < rects.size(); i++)
	{
		targets.push_back(Point(rects[i].x + rects[i].width / 2, rects[i].y + rects[i].height / 2));
	}
	sortTargets(targets);
}

bool sortY(Point a, Point b)
{
	return a.y < b.y;
}

void sortTargets(vector<Point> targets)
{
	std::sort(targets.begin(), targets.end(), sortY);
	//for (size_t i = 0; i < targets.size(); i++)
	//	printf("%d ", targets[i].y);
	//printf("\n");
}
