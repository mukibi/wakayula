
#include<stdio.h>

#include "opencv2/video/tracking.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"


using namespace cv;
using namespace std;


int main(int argc, char *argv[]) {

	int cntr = 0;
	VideoCapture capture;

	capture.open("video_surv.avi");

	if ( capture.isOpened() ) {

		Mat frame,prev_frame,edges;

		while ( true ) {

			capture >> frame;
			if (frame.empty()) break;
			imshow("Frame", frame);
			
			cntr++;
		}

	}

	else {
		printf("Could not open video_surv.avi");
	}

	return 0;

}

