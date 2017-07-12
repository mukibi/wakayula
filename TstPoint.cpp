#include<stdio.h>

//#include "opencv2/video/tracking.hpp"
//#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"


using namespace cv;
using namespace std;

int main() {

	VideoCapture capture;
	capture.open(0);

	if ( capture.isOpened() ) {


		Mat frame;

		for (int i = 0; i < 20; i++) {
			capture >> frame;
		}


		int center_x = frame.cols / 2;
		int center_y = frame.rows / 2;

		Point x(center_x, center_y);
		Point y(center_x, center_y);

		//y = x;
		//y.x++;
		//y.y++;

		if (x == y) {
			printf("Equal\n");
		}
		circle(frame, x, 10, Scalar(0, 0, 0), 2);

		imshow("Face", frame);
		waitKey(3000);
			
	}

	else {
		printf("Could not open video camera\n");
	}
	
}

