#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "face.h"
#include "opticalflow.h"
#include <iostream>
#include <stdio.h>
#include "motion.h"

using namespace std;
using namespace cv;

void Face::iniFaceTracking()
{
	//namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	if (!face_cascade.load(face_cascade_name)){ printf("--(!)Error loading face classifiers.\n"); return; };
	if (!body_cascade.load(body_cascade_name)){ printf("--(!)Error loading body classifiers.\n"); return; };
}
/*cascade face/body detection */
void Face::mainFaceTracking(Mat *frame)
{
	//std::vector<Rect> faces;
	faces.clear();
	targets.clear();
	facedetected = 0;
	bodydetected = 0;

	Mat frame_gray;

	cvtColor(*frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));  //minNeighbor used to be 2

	int i;
	for (i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		targets.push_back(center);
	}
	if (!faces.empty())
		facedetected = 1;
	if (faces.empty() && cascade)
	{
		body_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
		for (int i = 0; i < faces.size(); i++)
		{
			Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
			targets.push_back(center);
			bodydetected = 1;
		}
	}
	if (faces.empty())
		return;
	if (!facedetected)
	{
		x1 = faces.back().x - 0.25 * faces.back().width;  //faces.xy is the top-left-most coordinate
		if (x1 <= 0)
			x1 = 1;
		x2 = faces.back().x + 1.25 * faces.back().width;
		if (x2 >= frame->size().width)
			x2 = frame->size().width - 1;
		y1 = faces.back().y - 0.25 * faces.back().height;
		if (y1 <= 0)
			y1 = 1;
		y2 = faces.back().y + 1 * faces.back().height;
		if (y2 >= frame->size().height)
			y2 = frame->size().height - 1;
	}
	else
	{
		fx1 = faces.back().x - 2 * faces.back().width;  //faces.xy is the top-left-most coordinate
		if (fx1 <= 0)
			fx1 = 1;
		fx2 = faces.back().x + 3 * faces.back().width;
		if (fx2 >= frame->size().width)
			fx2 = frame->size().width - 1;
		fy1 = faces.back().y - 2 * faces.back().height;
		if (fy1 <= 0)
			fy1 = 1;
		fy2 = faces.back().y + 3 * faces.back().height;
		if (fy2 >= frame->size().height)
			fy2 = frame->size().height - 1;
	}
	/*cout << "x1=" << x1 << "; x2=" << x2 << endl;
	cout << "y1=" << y1 << "; y2=" << y2 << endl;*/

}
