#include "opencv2/highgui/highgui.hpp"
#include "color.h"
#include "motion.h"
#include "human.h"
#include "face.h"
#include "kalman.h"
#include "opticalflow.h"
#include <opencv\cv.h>
#include <iostream>
#include <sstream>
#include <string>
#include <deque>

using namespace cv;
using namespace std;

static void onMouse(int event, int x, int y, int, void* target)
{
	Point *p = (Point*)target;
	p->x = x;
	p->y = y;
}

int main(int argc, char* argv[])
{
	Color color; //for colormasking
	Motion motion; //for motiondetection
	Human human;
	Face face;
	Flow flow;

	/////////////////////////////////////////
	bool colortracking = FALSE;
	bool motiontracking = FALSE;
	bool recogtracking = TRUE;  //face+body
	bool manualtracking = FALSE;
	int firing = 0;  //gun should fire whenever this flag is 1
	int warning = 0;
	int curtarget = 0; //index of current target, 0 to maxtargets-1
	Point target(0, 0); //current target coordinate
	/////////////////////////////////////////

	bool humantracking = FALSE;
	bool facetracking = FALSE;
	int maxtargets = 3;
	face.cascade = 1; //0 only face, 1 face+body
	int NEARESTNEWTARGET = 20; //sum of pixels the new object have to be away from previous
	int WARNTIME = 30; //give the target this many frames to surrender
	int FIRETIME = 20; //shoot the target for this many frames
	int NEARESTNEIGHBOR = 200; //sum of pixels the objects have to be away from each other

	VideoCapture cap(0);// "Videos/ped.MOV"); // open the video camera no. 0
	//VideoCapture cap("Videos/KTH_test.mov");
	//VideoCapture cap("Videos/testwaving.mp4");
	//VideoCapture cap("Videos/interview.mp4");

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}
	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
	motion.FRAME_HEIGHT = dHeight;
	motion.FRAME_WIDTH = dWidth;
	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	vector<Point> targets;
	deque<Point> trajectory;
	Kalman kalmanfilter;
	deque<int> svote;  //used to count the number of surrender votes in the past second
	int newTargetVote = 0;  //used to kill noisy sudden target jumps
	int fvote = 0;  //used to count the time of warning to decide whether to shoot

	target = Point(dWidth / 2, dHeight / 2);
	trajectory.push_back(target);
	kalmanfilter.iniKalmanFilter(target);

	namedWindow("Capture");
	setMouseCallback("Capture", onMouse, &target);

	if (colortracking)
		color.iniColorTracking();
	motion.iniMotionTracking();
	face.iniFaceTracking();
	human.iniHumanTracking();

	while (1)
	{
		Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video
		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream." << endl;
			break;
		}

		targets.clear();
		if (colortracking)
		{
			color.mainColorTracking(&frame);
			targets = color.targets;
		}
		else if (motiontracking)
		{
			motion.mainMotionTracking(&frame);
			targets = motion.targets;
		}
		else if (humantracking)
		{
			human.mainHumanTracking(&frame);
			targets = human.targets;
		}
		else if (facetracking)
		{
			face.mainFaceTracking(&frame);
			targets = face.targets;
		}
		else if (recogtracking)
		{
			face.mainFaceTracking(&frame);
			targets = face.targets;
			if (!targets.empty())
			{
				sortTargets(targets);
				if (face.bodydetected)
					//if (!face.facedetected)
				{
					flow.xrange = Range(face.x1, face.x2);
					flow.yrange = Range(face.y1, face.y2);
					//rectangle(frame, Point(face.x1, face.y1), Point(face.x2, face.y2), Scalar(255, 0, 255), 2, 8, 0); //pink means body
				}
				else
				{
					flow.xrange = Range(face.fx1, face.fx2);
					flow.yrange = Range(face.fy1, face.fy2);
					//rectangle(frame, Point(face.fx1, face.fy1), Point(face.fx2, face.fy2), Scalar(0, 255, 255), 2, 8, 0);  //yellow means face
				}
				flow.flowDetector(&frame);
			}
		}
		else if (manualtracking)
		{
			targets.clear();
			targets.push_back(target);
		}

		/*get rid of close targets specified by NEARESTNEIGHBOR*/
		vector<int> toPop;  //back has bigger indexes
		toPop.clear();
		for (size_t i = 0; i < targets.size(); i++)
		for (size_t j = i + 1; j < targets.size(); j++)
		{
			if (abs(targets[i].x - targets[j].x) + abs(targets[i].y - targets[j].y) < NEARESTNEIGHBOR)
			{
				toPop.push_back(i);
				break;
			}
		}
		//cout << "toPop = " << toPop.size() << " targets = "<<targets.size()<<endl;
		while (!toPop.empty())
		{
			targets.erase(targets.begin() + toPop.back());
			toPop.pop_back();
		} //////////
		//cout << "after pop = " << targets.size() << endl;


		/*smooth target trajectory and display targets*/
		/*also get rid of sudden target noise*/
		if (!targets.empty())  //trajectory is the actual xy to sentry, target is just for display
		{
			if (curtarget < targets.size())
				target = targets[curtarget];
			else
			{
				int tempD = 9999;
				for (size_t i = 0; i < targets.size(); i++)  //find the closest one to the previous as the new target
				if (abs(trajectory.back().x - targets[i].x)
					+ abs(trajectory.back().y - targets[i].y)
					< tempD)
				{
					tempD = abs(trajectory.back().x - targets[i].x)
						+ abs(trajectory.back().y - targets[i].y);
					target = targets[i];
				}
				//target = targets.back(); //acquire target from multiple targets, only tracking target is red
			}
			trajectory.push_back(target);
		}

		if (trajectory.size() > 1)
		if (abs(trajectory.back().x - trajectory[trajectory.size() - 2].x)
			+ abs(trajectory.back().y - trajectory[trajectory.size() - 2].y)
				> NEARESTNEWTARGET)
		{
			if (newTargetVote >= 1)
			{
				newTargetVote = 0;
				//cout << "new target" << endl;
				//fvote--;
			}
			else
			{
				trajectory.back().x = trajectory[trajectory.size() - 2].x;
				trajectory.back().y = trajectory[trajectory.size() - 2].y;
				newTargetVote++; //cout << "vote++" << endl;
				fvote -= 3;
			}
		}
		if (!targets.empty())
		{
			Point tempP = trajectory.back(); trajectory.pop_back();
			trajectory.push_back(kalmanfilter.mainKalmanFilter(tempP));
			target = trajectory.back();
			circle(frame, target, 50, Scalar(0, 0, 255), 4, 8, 0);
		}

		if (targets.size() > 1)
		for (size_t i = 0; (i < targets.size() - 1) && (i < maxtargets - 1); ++i)
		{
			circle(frame, targets[i], 40, Scalar(0, 255, 0), 4, 8, 0);
		}
		for (size_t i = 0; i < trajectory.size() - 1; ++i) //draw trajectory
		{
			cv::line(frame, trajectory[i], trajectory[i + 1], Scalar(255, 0, 0), 4); //kalman filtered trajectory
			if (i > 20)
			{
				trajectory.pop_front();
			}
		}

		/*count surrender votes*/
		/*if surrendered, halt shooting*/
		if (svote.size() < flow.windowsize)
			svote.push_back(flow.surrender);
		else
		{
			svote.pop_front();
			svote.push_back(flow.surrender);
		}
		int svotesum = 0;
		if (fvote < 0)
			fvote = 0;
		for (size_t i = 0; i < svote.size(); i++)
			svotesum += svote[i];
		if (!targets.empty() && manualtracking == 0)
		{
			if (svotesum > flow.windowsize * 0.7)
			{
				putText(frame, "SURRENDER", target, FONT_HERSHEY_PLAIN, 2, Scalar(128, 255, 0), 3);
				firing = 0;
				warning = 0;
				fvote = 0;
			}
			else if (fvote > WARNTIME)
			{
				putText(frame, "FIRING", target, FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 3);
				firing = 1;
				fvote++;
				if (fvote > FIRETIME + WARNTIME)
					fvote = 0;
			}
			else
			{
				putText(frame, "WARNING", target, FONT_HERSHEY_PLAIN, 2, Scalar(0, 255, 255), 3);
				warning = 1;
				firing = 0;
				fvote++;
			}
		}
		else
		{
			fvote -= 2;
		}
		if (waitKey(30) == 32 && manualtracking)
		{
			firing = 1;
			warning = 0;
			putText(frame, "FIRING", target, FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 3);
			/*cout << "ESC key is pressed by user." << endl;
			break;*/
		}
		else if (manualtracking)
		{
			firing = 0;
			warning = 1;
		}
		imshow("Capture", frame);

	}
	return 0;

}