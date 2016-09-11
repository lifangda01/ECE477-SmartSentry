#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "kmeans.h"

using namespace cv;
using namespace std;

void Kmeans::mainKmeans(Mat *frame, vector<Point> *targets)
{
	//targets->clear();
	//Mat samples(frame->rows * frame->cols, 1, frame->type());
	//for (int y = 0; y < frame->rows; y++)
	//for (int x = 0; x < frame->cols; x++)
	////for (int z = 0; z < 3; z++)
	//samples.at<uchar>(y + x*frame->rows, 1) = frame->at<uchar>(y, x);

	//kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);

	//Mat new_image(src.size(), src.type());
	//for (int y = 0; y < src.rows; y++)
	//for (int x = 0; x < src.cols; x++)
	//{
	//	int cluster_idx = labels.at<int>(y + x*src.rows, 0);
	//	new_image.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);
	//	new_image.at<Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1);
	//	new_image.at<Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2);
	//}
	//imshow("clustered image", new_image);
}