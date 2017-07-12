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

	int maxCorners = 100;
	double qualityLevel = 0.01;
	double minDistance = 1;
  	int blockSize = 3;
  	bool useHarrisDetector = false;
 	double k = 0.04;

	int thresh = 200;
	int max_thresh = 255;

	int cntr = 0;
	vector<Rect> faces;
	vector<Point2f> points;

	float mean_core_shift_x = 0.0, mean_core_shift_y = 0.0;

	float dev_x = 0.0, dev_y = 0.0;

	if ( face_cascade.load("/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml") ) {

		if ( capture.isOpened()) {

			Mat frame,prev_frame,edges;

			while(true) {

				capture >> frame;
				remap(frame, frame, map1, map2, CV_INTER_LINEAR);

				Mat frame_gray;
				cvtColor( frame, frame_gray, CV_BGR2GRAY );
				equalizeHist( frame_gray, frame_gray );

				faces.clear();
				face_cascade.detectMultiScale( frame_gray, faces, 1.05, 6, 0|CV_HAAR_SCALE_IMAGE, Size(100, 100),Size() );

				if (faces.size() == 0) continue;

				for ( int i = 1; i < faces.size(); i++ ) {
					if (faces[i].width > faces[0].width) faces[0] = faces[i];
				}

				/// Detector parameters
				int blockSize = 2;
 				int apertureSize = 3;
				double k = 0.04;

					
				int face_rect_height = faces[0].height * 3;

				if ( (face_rect_height + faces[0].y) > frame_gray.rows )  {
					face_rect_height = frame_gray.rows - faces[0].y;
				}

				faces[0].height = face_rect_height;
	
				faces[0].x += faces[0].width / 6;
				faces[0].width *= (2.0/3.0);

				points.clear();
				goodFeaturesToTrack( frame_gray(faces[0]),points,maxCorners,qualityLevel,minDistance,Mat(),blockSize,useHarrisDetector,k );
				Point end_point = Point(faces[0].x + faces[0].width, faces[0].y + faces[0].height);
				rectangle(frame_gray, Point(faces[0].x, faces[0].y), end_point, Scalar(0,0,0));

			 	float mean_x = 0.0, mean_y = 0.0;
				float m2_x = 0.0, m2_y = 0.0;

				for (int i = 0; i < points.size(); i++) {

					
					int x = points[i].x + faces[0].x;
					int y = points[i].y + faces[0].y;

					float delta_x = (x-mean_x);
					float delta_y = (y-mean_y);

					mean_x += delta_x / (i+1);
					mean_y += delta_y / (i+1);

					m2_x += (delta_x * (x - mean_x));
					m2_y += (delta_y * (y - mean_y));

					circle(frame_gray, Point(x, y), 4, Scalar(0,0,0));

				}

				float n_dev_x = sqrtf(m2_x / (points.size() - 1));
				float n_dev_y = sqrtf(m2_y / (points.size() - 1));

				float mean_shift_x = 1.;
				if (dev_x > 0) {
					mean_shift_x = ((dev_x - n_dev_x) / dev_x) ;
				}
				mean_shift_x = mean_shift_x < 0 ? -mean_shift_x : mean_shift_x;


				float mean_shift_y = 1.;
				if (dev_x > 0) {
					mean_shift_y = ((dev_y - n_dev_y) / dev_y);
				}
				mean_shift_y = mean_shift_y < 0 ? -mean_shift_y : mean_shift_y;

				float mean_shift = (mean_shift_x + mean_shift_y) / 2;

				dev_x = n_dev_x;
				dev_y = n_dev_y;

				printf("Mean shift: %.3f\n", mean_shift);
				circle(frame_gray, Point(mean_x, mean_y), 10, Scalar(0,0,0));

				imshow("Face 1", frame_gray);

				//if (cntr++ > 10) break;
				waitKey(100);
		
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

