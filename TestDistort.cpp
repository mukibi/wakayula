#include<stdio.h>

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include<iostream>

using namespace cv;
using namespace std;

int main ( int argc, char *argv[] ) {

	Mat camera_matrix = ( Mat_<double>(3,3) << 1.0382848094367475e+03, 0., 2.1435568168393533e+02, 0., 1.0427237355898267e+03, 1.4072838519836708e+02, 0., 0., 1.  );
	Mat distortion_coefficients = (Mat_<double>(1,5) << -1.2701243756128322e-01, -1.6838683488627444e+00,-8.5735673026976001e-04, -1.0152173729324048e-02, 1.5291692116037700e+01);

	Mat map1,map2;
	initUndistortRectifyMap(camera_matrix, distortion_coefficients, Mat(), camera_matrix, Size(640, 480), CV_16SC2, map1,map2);

	VideoCapture capture;
	capture.open(0);
	//namedWindow("Face");
	Mat frame, frame_gray;
	CascadeClassifier face_cascade;
	std::vector<Rect> faces;

	if (! face_cascade.load("/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml") ) {
		cout << "Could not load cascade" << endl;
		return 0;
	}

	
	if ( capture.isOpened()) {
		while (1) {

			capture >> frame;
			cvtColor( frame, frame_gray, CV_BGR2GRAY );
			equalizeHist( frame_gray, frame_gray );

			face_cascade.detectMultiScale( frame_gray, faces, 1.05, 6, 0|CV_HAAR_SCALE_IMAGE, Size(100, 100),Size() );

			if (faces.size() > 0) {
				break;
			}
		}
	}

	else {
		cout << "Could not open camera" << endl;
		return 0;
	}

	Mat corrected = Mat::zeros(frame.cols, frame.rows, CV_8UC3);
	remap(frame_gray, frame_gray, map1, map2, CV_INTER_LINEAR);	

	imshow("Undistorted", frame_gray);
	waitKey(0);
	return 0;
	faces.clear();
	face_cascade.detectMultiScale( frame_gray, faces, 1.05, 6, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50),Size() );

	if ( faces.size() > 0 ) {
 
		printf("Distorted: Seen face at %d,%d running for %dx%d\n", faces[0].x, faces[0].y, faces[0].width, faces[0].height);
	}

	faces.clear();
	face_cascade.detectMultiScale( corrected, faces, 1.05, 6, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50),Size() );

	if ( faces.size() > 0 ) {
 
		printf("Undistorted: Seen face at %d,%d running for %dx%d\n", faces[0].x, faces[0].y, faces[0].width, faces[0].height);
	}

	imshow("Original", frame_gray);
	imshow("Undistorted", corrected);

	waitKey(0);
	capture.release();
	return 0;
}

