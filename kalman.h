#include <iostream>
#include <opencv2/opencv.hpp>

#ifndef KALMAN_H
#define KALMAN_H

using namespace cv;
using namespace std;

class Kalman{
private:
	KalmanFilter KF;
public:
	void iniKalmanFilter(Point ini);
	Point mainKalmanFilter(Point meas);
};

#endif