#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include<stdio.h>
#include<unistd.h>

using namespace cv;

int main(int argc, char *argv[]) {

	CvCapture *capture;
	Mat frame;
	namedWindow("Vid", WINDOW_AUTOSIZE);

	capture = cvCaptureFromCAM(-1);

	while (1) {

		frame = cvQueryFrame(capture);
		if (frame.data) {
			imshow("Vid", frame);
		}
		waitKey(0);
		sleep(1);
	}
	
	return(0);
}
