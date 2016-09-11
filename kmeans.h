#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef KMEANS_H
#define KMEANS_H

using namespace cv;
using namespace std;

class Kmeans{
private:
	Mat labels;
	Mat centers;
public:
	int clusterCount = 3;
	int attempts = 5;
	void mainKmeans(Mat *frame, vector<Point> *targets);
};
#endif