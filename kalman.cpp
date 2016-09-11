#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "kalman.h"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

void Kalman::iniKalmanFilter(Point ini){
	KF.init(4, 2, 0);
	KF.transitionMatrix = *(Mat_<float>(4, 4) << 1, 0, 1, 0,  0, 1, 0, 1,  0, 0, 1, 0,  0, 0, 0, 1);
	KF.statePre.at<float>(0) = ini.x;
	KF.statePre.at<float>(1) = ini.y;
	KF.statePre.at<float>(2) = 0;
	KF.statePre.at<float>(3) = 0;
	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
	setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(KF.errorCovPost, Scalar::all(.1));
}

Point Kalman::mainKalmanFilter(Point meas){
	Mat prediction = KF.predict();
	Mat_<float> measurement(2, 1); measurement.setTo(Scalar(0));
	measurement(0) = meas.x;
	measurement(1) = meas.y;
	Mat estimated = KF.correct(measurement);
	Point predictPt(estimated.at<float>(0), estimated.at<float>(1));
	return predictPt;
}
//
//void kalman(){
//	KalmanFilter KF(4, 2, 0);
//	KF.transitionMatrix = *(Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);
//	Mat_<float> measurement(2, 1); measurement.setTo(Scalar(0));
//
//	// init...
//	KF.statePre.at<float>(0) = mouse_info.x;
//	KF.statePre.at<float>(1) = mouse_info.y;
//	KF.statePre.at<float>(2) = 0;
//	KF.statePre.at<float>(3) = 0;
//	setIdentity(KF.measurementMatrix);
//	setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
//	setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
//	setIdentity(KF.errorCovPost, Scalar::all(.1));
//
//	// First predict, to update the internal statePre variable
//	Mat prediction = KF.predict();
//	Point predictPt(prediction.at<float>(0), prediction.at<float>(1));
//
//	// Get mouse point
//	measurement(0) = mouse_info.x;
//	measurement(1) = mouse_info.y;
//
//	Point measPt(measurement(0), measurement(1));
//
//	// The "correct" phase that is going to use the predicted value and our measurement
//	Mat estimated = KF.correct(measurement);
//	Point statePt(estimated.at<float>(0), estimated.at<float>(1));
//}