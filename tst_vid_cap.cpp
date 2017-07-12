#include<stdio.h>
#include<limits.h>

//#include "opencv2/video/tracking.hpp"
#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {

	VideoCapture capture;
	int frame_cntr = -1;
	capture.open("video_surv.avi");
	//capture.open(0);

	if ( capture.isOpened() ) {
		if (argc > 2) {
			capture.set(CAP_PROP_FPS, 1); 
		}

		Mat frame;
		while ( true ) {

			capture >> frame;
			if (frame.empty()) break;
			
			frame_cntr++;

			//printf("%d\n", frame_cntr);

		}
	}

	return 0;
}

