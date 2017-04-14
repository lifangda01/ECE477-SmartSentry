#include <iostream>
#include <opencv2/opencv.hpp>

#ifndef OPTICALFLOW_H
#define OPTICALFLOW_H

using namespace cv;
using namespace std;

class Flow{
private:
	Mat current, previous;
	deque<vector<double>> window;
public:
	Mat flowmap;
	const int UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3;
	int resizescale = 10;
	Range xrange, yrange;
	int windowsize = 10;  //normally a waving action takes 30 frames
	int surrender = 0;  //0 is not surrendering, 1 is surrendering

	void flowDetector(Mat *frame);
	void drawOptFlowMap(const Mat& flow, Mat* cflowmap, int step, const Scalar& color);
	vector<double> flowHist(Mat& flow, Mat* frame, Range yrange, Range xrange, int step);
	vector<double> flowGradient(vector<double> hist, int windowsize);
};

#endif