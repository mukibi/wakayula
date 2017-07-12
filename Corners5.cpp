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

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {

	/// Global variables
	Mat src, src_gray;
	CascadeClassifier face_cascade;

	Mat camera_matrix = ( Mat_<double>(3,3) << 1.0382848094367475e+03, 0., 2.1435568168393533e+02, 0., 1.0427237355898267e+03, 1.4072838519836708e+02, 0., 0., 1.  );
	Mat distortion_coefficients = (Mat_<double>(1,5) << -1.2701243756128322e-01, -1.6838683488627444e+00,-8.5735673026976001e-04, -1.0152173729324048e-02, 1.5291692116037700e+01);

	Mat map1,map2;
	initUndistortRectifyMap(camera_matrix, distortion_coefficients, Mat(), camera_matrix, Size(640, 480), CV_16SC2, map1,map2);


	VideoCapture capture;

	capture.open(0);

	//namedWindow("Face");
	vector<Rect> faces;
	vector<Point2f> points;

	int maxCorners = 100;
	double qualityLevel = 0.01;
	double minDistance = 1;
  	int blockSize = 3;
  	bool useHarrisDetector = false;
 	double k = 0.04;


	vector<Mat> unmatched_frames;

	if ( face_cascade.load("/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml") ) {

		if ( capture.isOpened()) {

			Mat frame,prev_frame,frame_gray_dup;

			while(true) {

				capture >> frame;
				remap(frame, frame, map1, map2, CV_INTER_LINEAR);

				Mat frame_gray;
				cvtColor( frame, frame_gray, CV_BGR2GRAY );
				equalizeHist( frame_gray, frame_gray );


				faces.clear();
				face_cascade.detectMultiScale( frame_gray, faces, 1.05, 6, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30),Size() );
		

				if (faces.size() == 0)  {
					unmatched_frames.push_back(frame_gray);
					continue;
				}

				
				for ( int i = 1; i < faces.size(); i++ ) {
					if (faces[i].width > faces[0].width) faces[0] = faces[i];
				}

			//continue;

				points.clear();

				minDistance = faces[0].width / 10;

				goodFeaturesToTrack( frame_gray(faces[0]), points, maxCorners,qualityLevel,minDistance,Mat(),blockSize,useHarrisDetector,k );


				frame_gray.copyTo(frame_gray_dup);
				Point end_point = Point(faces[0].x + faces[0].width, faces[0].y + faces[0].height);
				rectangle(frame_gray_dup, Point(faces[0].x, faces[0].y), end_point, Scalar(0,0,0));

				for (int i = 0; i < points.size(); i++) {

					points[i].x += faces[0].x;
					points[i].y += faces[0].y;

					circle(frame_gray_dup, points[i], 4, Scalar(0,0,0));

				}
			
				imshow("Face", 	frame_gray_dup);

				
				if (unmatched_frames.size() > 0) {

					printf("Unmatched: %d\n", unmatched_frames.size());

					vector<Point2f> prev_pts = points;
					vector<Point2f> next_pts; 
					vector<uchar> status;
					vector<float> err;

					prev_frame = frame_gray;

					for ( int i = unmatched_frames.size() - 1; i > -1; i-- ) {

						calcOpticalFlowPyrLK(prev_frame, unmatched_frames[i], prev_pts , next_pts, status, err);
					

						int matched = 0;

						for ( int j = 0; j < status.size(); j++ ) {
							if (status[j] == 1) matched++;	
						}	


						//if match ratio is 67% plus, pass
						float match_ratio = matched / (float) prev_pts.size();

						if (match_ratio > 0.67) {

							prev_frame = unmatched_frames[i];
							Mat unseen_frame;
							prev_frame.copyTo(unseen_frame);
						
							for (int j = 0; j < next_pts.size(); j++) {
								circle(unseen_frame, next_pts[j], 4, Scalar(0,0,0));
							}

							prev_pts = next_pts;
							imshow("Face 0", unseen_frame);	
							waitKey(100);
						}
						else {
							printf("No match, disregarding\n");
						}
						
					}

				}


				unmatched_frames.clear();
	

				//printf("%d, %.3f: %d\n", faces[0].width, minDistance, points.size());
				
				waitKey(10);

			}

			capture.release();

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
