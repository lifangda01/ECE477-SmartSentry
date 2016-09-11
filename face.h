#include <iostream>
#include <opencv2/opencv.hpp>
#include "opticalflow.h"

#ifndef FACE_H
#define FACE_H

using namespace cv;
using namespace std;

class Face{
private:
	CascadeClassifier face_cascade;
	CascadeClassifier body_cascade;

public:
	string window_name = "Body/Face Detection";

	String face_cascade_name = "Features/haarcascade_frontalface_alt.xml"; //works well
	//String body_cascade_name = "Features/haarcascade_mcs_upperbody.xml";
	//String body_cascade_name = "Features/haarcascade_fullbody.xml";
	String body_cascade_name = "Features/haarcascade_profileface.xml";

	int cascade = 1;
	std::vector<Rect> faces;
	std::vector<Point> targets;
	Flow flow;
	int x1, x2, y1, y2;  //for body size rectangle
	int fx1, fx2, fy1, fy2;  //for face size rectangle
	int facedetected = 0;
	int bodydetected = 0;

	void iniFaceTracking();
	void mainFaceTracking(Mat *frame);
};

#endif