#include "opencv2/highgui/highgui.hpp"
#include <opencv\cv.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace cv;
using namespace std;

#ifndef COLOR_H
#define COLOR_H

class  Morph_size{public: int ero_size = 3; int dil_size = 8;};

class Color{
public:
	int H_MIN = 24;  //for green tracking
	int H_MAX = 86;
	int S_MIN = 45;
	int S_MAX = 170;
	int V_MIN = 181;
	int V_MAX = 245;
	int FRAME_WIDTH = 640;
	int FRAME_HEIGHT = 480;
	int MAX_NUM_OBJECTS = 50;
	const int MIN_OBJECT_AREA = 20 * 20;
	const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
	Morph_size morph_size;
	vector<Point> targets;

	string trackbarWindowName = "Color Trackbars";
	string window_name = "Color Tracking";
	string window_name2 = "Color Mask";

	void iniColorTracking();
	void on_trackbar(int, void*);
	string intToString(int number);
	void createTrackbars();
	void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed);
	void mainColorTracking(Mat *frame);
};
#endif

void drawObject(int x, int y, Mat &frame);
void morphOps(Mat &frame, Morph_size morph_size);
