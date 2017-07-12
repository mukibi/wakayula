#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include<stdio.h>
#include<sys/time.h>

#include<iostream>
#include<sstream>

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {

	struct timeval time;

	gettimeofday(&time, NULL);

	time_t start_time = time.tv_sec;

	Mat frame,fgMaskMOG2;
	Ptr<BackgroundSubtractor> pMOG2;

	pMOG2 = createBackgroundSubtractorMOG2();

	VideoCapture capture(0);

	int cntr = 0;

	for (cntr; ; cntr++) {
	
		capture >> frame;
		cvtColor(frame, frame, COLOR_BGR2GRAY);
		//GaussianBlur(frame,frame, Size(5,5), 3,3);

		pMOG2->apply(frame, fgMaskMOG2);
		//every 10 frames check if we have been running for 5 secs
		//and quit
		if ( cntr % 30 == 0 ) {

			gettimeofday(&time, NULL);
			time_t secs = time.tv_sec;

			if (secs - start_time > 15) {
				break;
			}
		}
	}

	if (fgMaskMOG2.data) {
		imwrite("fg_mask.png", fgMaskMOG2);
	}
	else {
		printf("No FG mask created\n");
	}

	return 0;
}

