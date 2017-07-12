/**
 * @function goodFeaturesToTrack_Demo.cpp
 * @brief Demo code for detecting corners using Shi-Tomasi method
 * @author OpenCV team
 */
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

	/// Global variables
	Mat src, src_gray;

	Mat camera_matrix = ( Mat_<double>(3,3) << 1.0382848094367475e+03, 0., 2.1435568168393533e+02, 0., 1.0427237355898267e+03, 1.4072838519836708e+02, 0., 0., 1.  );
	Mat distortion_coefficients = (Mat_<double>(1,5) << -1.2701243756128322e-01, -1.6838683488627444e+00,-8.5735673026976001e-04, -1.0152173729324048e-02, 1.5291692116037700e+01);

	Mat map1,map2;
	initUndistortRectifyMap(camera_matrix, distortion_coefficients, Mat(), camera_matrix, Size(640, 480), CV_16SC2, map1,map2);

	VideoWriter writer;
	VideoCapture capture;

	int cntr = 0;

        if ( !writer.open("video_surv2.avi", CV_FOURCC('X', 'V', 'I', 'D'), 25, Size(640, 480)) ) {

		printf("Could not open VideoWriter\n");
		return -1;

	}


	capture.open(0);

	if ( capture.isOpened()) {

		Mat frame,prev_frame,edges;

		while(true) {

			capture >> frame;
			remap(frame, frame, map1, map2, CV_INTER_LINEAR);

			writer.write(frame);
			waitKey(10);
		
			if (cntr++ > 1000) break;

		}

		capture.release();
		writer.release();
	}

	else {
		printf("Could not open camera\n");
	}

	return 0;
}

