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
#include<list>

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
	vector<Point2f> prev_pts;
	list<Point2f> cores;

	float scale_ratio = 1.0;
	float init_radius = 1.0;
	
	int cores_vector_size = 0;

	if ( face_cascade.load("/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml") ) {

		if ( capture.isOpened()) {

			Mat frame,prev_frame,frame_gray_dup;
			while(true) {

				capture >> frame;
				remap(frame, frame, map1, map2, CV_INTER_LINEAR);

				Mat frame_gray;
				cvtColor( frame, frame_gray, CV_BGR2GRAY );
				equalizeHist( frame_gray, frame_gray );

				if (cntr == 0) {
					faces.clear();
					face_cascade.detectMultiScale( frame_gray, faces, 1.05, 6, 0|CV_HAAR_SCALE_IMAGE, Size(100, 100),Size() );
		

					if (faces.size() == 0)  {
						cntr = 0;
						continue;
					}

					for ( int i = 1; i < faces.size(); i++ ) {
						if (faces[i].width > faces[0].width) faces[0] = faces[i];
					}
					
					int face_rect_height = faces[0].height * 3;

					if ( (face_rect_height + faces[0].y) > frame_gray.rows )  {
						face_rect_height = frame_gray.rows - faces[0].y;
					}
					faces[0].height = face_rect_height;
	
					faces[0].x += faces[0].width / 6;
					faces[0].width *= (2.0/3.0);
			
					prev_pts.clear();
					goodFeaturesToTrack( frame_gray(faces[0]),prev_pts,maxCorners,qualityLevel,minDistance,Mat(),blockSize,useHarrisDetector,k );

					frame_gray.copyTo(frame_gray_dup);
					Point end_point = Point(faces[0].x + faces[0].width, faces[0].y + faces[0].height);
					rectangle(frame_gray_dup, Point(faces[0].x, faces[0].y), end_point, Scalar(0,0,0));

					float mean_x=0.0, mean_y = 0.0;
					float m2_x = 0.0, m2_y = 0.0;

					for (int i = 0; i < prev_pts.size(); i++) {

						prev_pts[i].x += faces[0].x;
						prev_pts[i].y += faces[0].y;

						float delta_x = (prev_pts[i].x - mean_x);
						float delta_y = (prev_pts[i].y - mean_y);

						mean_x += delta_x / (float) (i+1);
						mean_y += delta_y / (float) (i+1);

						m2_x += (delta_x * (prev_pts[i].x - mean_x));
						m2_y += (delta_y * (prev_pts[i].y - mean_y));

						circle(frame_gray_dup, Point(prev_pts[i].x, prev_pts[i].y), 4, Scalar(0,0,0));

					}

					float n_dev_x = sqrtf(m2_x / (prev_pts.size() - 1));
					float n_dev_y = sqrtf(m2_y / (prev_pts.size() - 1));


					//Calculate radius
					init_radius = 0.0;

					for ( int i = 0; i < prev_pts.size(); i++ ) {

						float delta_x = fabsf(mean_x - prev_pts[i].x); 
						float delta_y = fabsf(mean_y - prev_pts[i].y); 

						if (delta_x > n_dev_x) continue;
						if (delta_y > n_dev_y) continue;

						float delta = sqrtf(powf(delta_x, 2) + powf(delta_y, 2));

						init_radius += (delta - init_radius) / (float) (i+1);

					}

					printf("Init radius: %.3f\n", init_radius);

					imshow("Seen face", frame_gray_dup);
				}
				else {
						
					vector<Point2f> next_pts; 
					vector<uchar> status;
					vector<float> err;

					calcOpticalFlowPyrLK(prev_frame, frame_gray, prev_pts , next_pts, status, err);
					int matched = 0;

					for ( int i = 0; i < status.size(); i++ ) {
						if (status[i] == 1) matched++;	
					}	


					//if match ratio is 67% plus, pass
					float match_ratio = matched / (float) prev_pts.size();
					//printf("Next points: %d; matched: %d; prev_pts: %d; match ratio: %f\n", next_pts.size(), matched, prev_pts.size(), match_ratio);
					frame_gray.copyTo(frame_gray_dup);
					if ( match_ratio < 0.67 ) {
					
						printf("No match\n");
						//break;

						//find rectangle
						float new_width  = faces[0].width * scale_ratio;
						float new_height = faces[0].height * scale_ratio;

						printf("%.3f || %.3f (%.2f)\n", new_width, (float)faces[0].width, scale_ratio);
						float min_x = -1, max_x = 0.0;
						float min_y = -1, max_y = 0.0;

						for ( list<Point2f>::iterator it = cores.begin(); it != cores.end(); ++it ) {

							Point2f pt = *it;

							if (min_x < 0) {

								min_x = pt.x;
								min_y = pt.y;

								max_x = pt.x;
								max_y = pt.y;
							}
							else {
								if (pt.x < min_x) {
									min_x = pt.x;
								}
								if (pt.y < min_y) {
									min_y = pt.y;
								}

								if (pt.x > max_x) {
									max_x = pt.x;
								}
								if (pt.y > max_y) {
									max_y = pt.y;
								}
							}

						}

						float start_x = min_x - (new_width / 2);
						float start_y = min_y - (new_height / 2);

						if (start_x < 0) start_x = 0;
						if (start_y < 0) start_y = 0;

						float stop_x = max_x + (new_width / 2); 
						float stop_y = max_y + (new_height / 2);

						if (stop_x > frame_gray.cols) stop_x = frame_gray.cols;
						if (stop_y > frame_gray.rows) stop_y = frame_gray.rows;

						rectangle(frame_gray_dup, Point2f(start_x, start_y), Point2f(stop_x, stop_y), Scalar(0,0,0));

						circle(frame_gray_dup, Point2f(min_x, min_y), 6, Scalar(0,0,0));
						circle(frame_gray_dup, Point2f(max_x, max_y), 6, Scalar(0,0,0));

						imshow("Fail", frame_gray_dup);
						waitKey(10000);
						break;
					}

					else {

						float mean_x=0.0, mean_y = 0.0;
						float m2_x = 0.0, m2_y = 0.0;

						for (int i = 0; i < next_pts.size(); i++) {

							float delta_x = (next_pts[i].x - mean_x);
							float delta_y = (next_pts[i].y - mean_y);

							mean_x += delta_x / (float) (i+1);
							mean_y += delta_y / (float) (i+1);

							m2_x += (delta_x * (next_pts[i].x - mean_x));
							m2_y += (delta_y * (next_pts[i].y - mean_y));

							circle(frame_gray_dup, next_pts[i], 2, Scalar(0,0,0));
						}

						float n_dev_x = sqrtf(m2_x / (next_pts.size() - 1));
						float n_dev_y = sqrtf(m2_y / (next_pts.size() - 1));


						if (++cores_vector_size > 5) {
							cores.pop_front();
						}

						cores.push_back(Point(mean_x, mean_y));

						//for ( list<Point2f>::iterator it = cores.begin(); it != cores.end(); ++it ) {
						//	circle(frame_gray_dup, *it, 4, Scalar(0,0,0));
						//}

						//Calculate radius
						float radius = 0.0;
						float accepted = 0.0;

						for ( int i = 0; i < next_pts.size(); i++ ) {

							//only accept pts that are no more than
							//2 std devs away
							
							float delta_x = fabsf(mean_x - next_pts[i].x); 
							float delta_y = fabsf(mean_y - next_pts[i].y); 

							if (delta_x > n_dev_x) continue;
							if (delta_y > n_dev_y) continue;
							accepted++;

							float delta = sqrtf(powf(delta_x, 2) + powf(delta_y, 2));

							radius += (delta - radius) / (float) (i+1);

						}

						printf("Radius: %.3f; Pass rate: %.3f\n", radius, (accepted/next_pts.size()));

						scale_ratio = radius / init_radius;
						prev_pts = next_pts;

						imshow("Seen face", frame_gray_dup);
					}

				}

				prev_frame = frame_gray;
				cntr++;
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

