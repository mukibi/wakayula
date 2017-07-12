/**
 * @function goodFeaturesToTrack_Demo.cpp
 * @brief Demo code for detecting corners using Shi-Tomasi method
 * @author OpenCV team
 */
#include<stdio.h>
#include<math.h>

#include "opencv2/video/tracking.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"

//#include <iostream>
#include<list>
#include<unistd.h>

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {

	VideoCapture capture;
	Mat frame;

	vector<Mat> mats;

	capture.open(0);

	for (int i = 0; i < 10; i++) {

		capture >> frame;
		Mat dup;
		frame.copyTo(dup);
		mats.push_back(dup);

		//imshow("Face", frame);
		//sleep(1);
		waitKey(1000);

	}

	for (int i = 0; i < mats.size(); i++) {
		if (mats[i].empty()) {
			printf("Invalid mat\n");
		}
		else {
			imshow("Face", mats[i]);
			waitKey(1000);
			printf("Printf valid mat: %dx%d\n", mats[i].cols, mats[i].rows);
		}
	}

}
