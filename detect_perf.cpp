#include<stdio.h>
#include<stdlib.h>
#include<time.h>

//#include "opencv2/video/tracking.hpp"
#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/core/ocl.hpp"

using namespace cv;
using namespace std;
using namespace cv::ocl;

int main ( int argc, char *argv[] ) {

	int maxCorners = 100;
	double qualityLevel = 0.01;
	double minDistance = 1;
  	int blockSize = 3;
  	bool useHarrisDetector = false;
 	double k = 0.04;

	CascadeClassifier face_cascade;
	vector<Rect> faces;
	vector<Point2f> points;

	long mean_ticks = 0;

	if ( face_cascade.load("/usr/share/OpenCV/lbpcascades/lbpcascade_frontalface.xml") ) {

		VideoCapture capture;
		int frame_cntr = 0;
		capture.open("video_surv.avi");
		int mean_ticks = 0;
		clock_t ticks_before, ticks_after;

		if ( capture.isOpened() ) {

			Mat frame,frame_gray,frame_gray_2, frame_gray_3;

			while ( true ) {
			
				capture >> frame;
				if (frame.empty()) break;
			
				frame_cntr++;
				cvtColor( frame, frame_gray_2, CV_BGR2GRAY );
				equalizeHist( frame_gray_2, frame_gray_3 );
			
				resize(frame_gray_3, frame_gray, Size(), 0.5, 0.5);

				faces.clear();
				clock_t before = clock();
				face_cascade.detectMultiScale( frame_gray, faces, 1.05, 6, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30),Size() );
				clock_t after = clock();

				long num_ticks = after - before;
				mean_ticks += ((num_ticks - mean_ticks) /  frame_cntr);

				if ( faces.size() > 0 )  {

					Mat frame_dup;
					frame_gray.copyTo(frame_dup);

					for ( int i = 0; i < faces.size(); i++ ) {

						Point start(faces[i].x, faces[i].y);
						Point stop(faces[i].x + faces[i].width, faces[i].y + faces[i].height);

						rectangle(frame_dup, start, stop, Scalar(0,0,0), 3);

					}
					//imshow("Faces", frame_dup);
					//waitKey(500);
					//printf("%d\n", frame_cntr-1);
				}
	

			}

			capture.release();
			printf("Mean ticks: %d", mean_ticks);

		}

		else {
			printf("Could not open camera\n");
		}

	}
	else {
		printf("Could not load haarcascade\n");
	}

	return 0;
}


