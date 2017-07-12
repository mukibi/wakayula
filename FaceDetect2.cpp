#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <unistd.h>

using namespace std;
using namespace cv;

/** @function main */
int main( int argc, const char** argv ) {

	int maxCorners = 100;
	double qualityLevel = 0.01;
	double minDistance = 1;
  	int blockSize = 3;
  	bool useHarrisDetector = false;
 	double k = 0.04;

	CascadeClassifier face_cascade;
	vector<Rect> faces;
	vector<Point2f> points;

	if ( face_cascade.load("/usr/share/OpenCV/lbpcascades/lbpcascade_frontalface.xml") ) {

		VideoCapture capture;
		int frame_cntr = -1;
		capture.open("/home/k0k1/wakayula/pizza_counter/vid.mkv");

		//int mean_ticks = 0;
		//clock_t ticks_before, ticks_after;

		if ( capture.isOpened() ) {

			Mat frame,frame_gray;

			while ( true ) {
			
				capture >> frame;

				if ( frame.empty() ) break;
				//resize( frame, frame, Size(frame.cols / 2, frame.rows / 2) );

				frame_cntr++;
				cvtColor( frame, frame_gray, CV_BGR2GRAY );
				equalizeHist( frame_gray, frame_gray );

				faces.clear();

				face_cascade.detectMultiScale( frame_gray, faces, 1.05, 6, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30),Size(60,60) );


				if ( faces.size() > 0 )  {

					for ( int i = 0; i < faces.size(); i++ ) {

						printf("Face: %dx%d\n", faces[i].width, faces[i].height);

						Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
						ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0);
						imshow("Face", frame);
						waitKey(500);

					}

				}

			}

		}
		else {
			printf("Could not open VideoCapture");
		}
	}
	else {
		printf("Could not load LBP cascade");
	}

	return 0;
}


