#include <iostream>
#include "human.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void Human::iniHumanTracking(){
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	//namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	return;
}

void Human::mainHumanTracking(Mat *frame)
{
	//namedWindow("video capture", CV_WINDOW_AUTOSIZE);
	targets.clear();
	vector<Rect> found, found_filtered;
	hog.detectMultiScale(*frame, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);

	size_t i, j;
	for (i = 0; i < found.size(); i++)
	{
		Rect r = found[i];
		//for (j = 0; j < found.size(); j++)
		//	if (j != i && (r & found[j]) == r)
		//		break;
		//if (j == found.size())
			found_filtered.push_back(r);
	}
	for (i = 0; i < found_filtered.size(); i++)
	{
		targets.push_back(Point(found_filtered[i].x + found_filtered[i].width / 2, found_filtered[i].y + found_filtered[i].height/2));
	}
}