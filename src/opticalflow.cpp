#include <iostream>
#include <opencv2/opencv.hpp>
#include <deque>
#include "opticalflow.h"

void Flow::flowDetector(Mat *frame)
{
	current = frame->clone();
	//resize(current, current, Size(current.size().width / resizescale, current.size().height / resizescale));
	cvtColor(current, current, CV_BGR2GRAY);
	if (previous.empty())
	{
		previous = current.clone();
		return;
	}
	calcOpticalFlowFarneback(previous, current, flowmap, 0.5, 3, 15, 3, 5, 1.1, 0);
	//drawOptFlowMap(flowmap, frame, 10, CV_RGB(0, 255, 0));

	/*waving determination*/
	Range upperrange(yrange.start, yrange.start + (yrange.end - yrange.start) / 4);
	//Range sidesrange(yrange.start + (yrange.end - yrange.start) / 4, yrange.start + (yrange.end - yrange.start) /2);

	vector<double> upper = flowGradient(flowHist(flowmap, frame, upperrange, xrange, 10),windowsize);
	double area = (yrange.end - yrange.start) * (xrange.end - xrange.start)/4000;
	
	/*normalize gradient data with time window size and rectangle area size*/
	for (size_t i = 0; i < upper.size(); i++)
		upper[i] = upper[i] / (windowsize*area);
	//cout << "UPPER: UP=" << upper[0] << " DOWN=" << upper[1] << " LEFT=" << upper[2] << " RIGHT=" << upper[3] << endl;
	//cout << "AREA=" << area << endl;

	/*calculate the cross-correlation between upper and sides*/ 
	if (((upper[LEFT] + upper[RIGHT] + upper[UP] > 5 * upper[DOWN]) && upper[UP] > 3
		|| (upper[LEFT] + upper[RIGHT] + upper[DOWN] > 5 * upper[UP]) && upper[DOWN] > 3) //relative motion
		&& upper[LEFT] > 3  //absolute motion
		&& upper[RIGHT] > 3
		)
	{
		surrender = 1;
	}
	else
	{
		surrender = 0;
	}
	previous = current.clone();
}

void Flow::drawOptFlowMap(const Mat& flow, Mat* frame, int step, const Scalar& color) {
	for (int y = 0; y < frame->rows; y += step)
	for (int x = 0; x < frame->cols; x += step)
	{
		const Point2f& fxy = flow.at< Point2f>(y, x);
		line(*frame, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), color, 1);
		circle(*frame, Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), 1, color, -1);
	}
}

vector<double> Flow::flowHist(Mat& flow, Mat* frame, Range yrange, Range xrange, int step)
{
	vector<double> hist; //0-up 1-down 2-left 3-right
	hist.assign(4, 0);
	for (int y = yrange.start; y < yrange.end; y += step)
	for (int x = xrange.start; x < xrange.end; x += step)
	{
		const Point2f& fxy = flow.at< Point2f>(y, x);
		if (fxy.x >= 0)
			hist[RIGHT] += fxy.x;
		else
			hist[LEFT] -= fxy.x;
		if (fxy.y >= 0)
			hist[DOWN] += fxy.y;
		else
			hist[UP] -= fxy.y;
	}
	//cout << "UP=" << hist[0] << " DOWN=" << hist[1] << " LEFT=" << hist[2] << " RIGHT=" << hist[3] <<endl;
	return hist;
}

vector<double> Flow::flowGradient(vector<double> hist, int windowsize)  //window size decides how many frames to take for hist
{
	vector<double> gradient; //0-up 1-down 2-left 3-right
	//static deque<vector<double>> window;
	if (window.size() < windowsize)
		window.push_back(hist);
	else
	{
		window.pop_front();
		window.push_back(hist);
	}
	gradient.assign(4, 0);
	for (size_t i = 0; i < window.size(); i++)
	for (size_t j = 0; j < hist.size(); j++)
		gradient[j] += window[i][j];
	return gradient;
}