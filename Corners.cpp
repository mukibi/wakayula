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

	if ( face_cascade.load("/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml") ) {

		if ( capture.isOpened()) {

			Mat frame,prev_frame;
			bool initd = false;
			Rect search_area;
			Rect face_core;
			vector<Point2f> prev_pts;

			Point core = Point(0,0);

			int maxCorners = 40;
			double qualityLevel = 0.01;
			double minDistance = 1;
  			int blockSize = 3;
  			bool useHarrisDetector = false;
 			double k = 0.04;

	
			//allow up to 5-frames to re-find object
			int fails = 0;
			int cntr_y_shift = 0;
			int cntr_x_shift = 0;

			int cntr = 0;
			float x_shift_mean = 0;
			float x_shift_m2 = 0;
			float x_shift_upper_lim = 0;

			float y_shift_mean = 0;
			float y_shift_m2 = 0;
			float y_shift_upper_lim = 0;

			while(true) {

				capture >> frame;

				//undistort
				remap(frame, frame, map1, map2, CV_INTER_LINEAR);	
				Mat frame_gray;
				cvtColor( frame, frame_gray, CV_BGR2GRAY );
				equalizeHist( frame_gray, frame_gray );

				if (! initd) {


					Size frame_size = frame.size();	
					std::vector<Rect> faces;


					///-- Detect faces
					face_cascade.detectMultiScale( frame_gray, faces, 1.05, 6, 0|CV_HAAR_SCALE_IMAGE, Size(100, 100),Size() );

					if (faces.size() == 0) continue;

					for (int i = 1; i < faces.size(); i++) {
						if (faces[i].width > faces[0].width) faces[0] = faces[i];
					}

					//triple rect height
					int face_rect_height = faces[0].height * 3;

					//if (face_rect_height < 100) continue;

					//should exceed the size of the frame
					if ( (face_rect_height + faces[0].y) > frame_size.height )  {
						face_rect_height = frame_size.height - faces[0].y;
					}

					//printf("Before rescale, height is %d\n", faces[0].height);
					faces[0].height = face_rect_height;
					//printf("After rescale, height is %d\n", faces[0].height);
					//face_rect_height

					int face_core_height = (faces[0].height / 18) * 17;
					int face_core_width  =  faces[0].width / 2;

					if (face_core_height < 30) continue;

					face_core = Rect( faces[0].x + face_core_width / 2, faces[0].y + (face_core_height / 17), face_core_width, face_core_height);

					//int num_cor
					//imshow("Face", face_core);
					vector<Point2f> corners;
					//minDistance = face_core_width / 5;
 
					//need to execute goodFeaturesToTrack() twice:
					//first to find the edge of the face by executing
					//it against the entire Rect and second to find
					//the points within the face 'core' that will
					//actually be tracked.
					///1st call to goodFeaturesToTrack(): find the face edge
					goodFeaturesToTrack( frame_gray(faces[0]),corners,maxCorners,qualityLevel,minDistance,Mat(),blockSize,useHarrisDetector,k );
	
					//calculate face radius as well
					float sum_y = 0, sum_x = 0;

					for (int j = 0; j < corners.size(); j++) {

						sum_x += corners[j].x;
						sum_y += corners[j].y;

						//circle(frame_gray, pt, 4, Scalar(255,255,255));

					}

					//printf("Min x: %d; Min y: %d; sum_x: %d; sum_y: %d\n", min_x, min_y, sum_x, sum_y);

					float core_x = (sum_x / corners.size());
					float core_y = (sum_y / corners.size());

					core = Point((int) (core_x + faces[0].x), (int) (core_y + faces[0].y));
					//circle(frame_gray, core, 16, Scalar(0,0,0), 3);

					imshow("Face 1", frame_gray(face_core));
					//imshow("Face 2", frame_gray(faces[0]));
					//imshow("Face 3", frame_gray);
					//waitKey(0);
					printf("Core is at %d,%d\n", core.x, core.y);

					
					initd = true;
					search_area = faces[0];
					frame_gray.copyTo(prev_frame);
					//second call to goodFeaturesToTrack(): find the points to track
					goodFeaturesToTrack( frame_gray(face_core),prev_pts,maxCorners,qualityLevel,minDistance,Mat(),blockSize,useHarrisDetector,k );
					if (prev_pts.size() < 40) {
						printf("Breaking out on too few points: %d\n", prev_pts.size());
						//break;
					}

					printf("Initd with %d points\n", prev_pts.size());
					//printf("Offset is %d,%d\n", search_area.x, search_area.y);

					}
					else {

						//imshow("Face 2", frame_gray(search_area));

						vector<Point2f> next_pts; 
						vector<uchar> status;
						vector<float> err;

						calcOpticalFlowPyrLK(prev_frame(search_area), frame_gray(search_area), prev_pts , next_pts, status, err);
						int matched = 0;

						for ( int i = 0; i < status.size(); i++ ) {
							if (status[i] == 1) matched++;	
						}	


						//imshow("Face 2", frame_gray(search_area));
						//circle(frame_gray, core, 16, Scalar(0,0,0), 3);
						//imshow("Face 2", frame_gray);
						//waitKey(0);

						//if match ratio is 67% plus, pass
						float match_ratio = matched / prev_pts.size();
						if (match_ratio < 0.67) {
							printf("No match. Incrementing fails...\n");
							if (fails++ >= 25) {
								break;
							}
							continue;
							//break;
						}
						fails = 0;
						//compute next points
						//seeing a problem with double
						//re-calculation
						//I re-center the search box
						//based on the previous points
						//then I find the next points and find a new 
						//center in the process
						//Maybe this will allow continuity
						//but meanwhile, I don't see any other
						//way out
						prev_pts.clear();
						goodFeaturesToTrack( frame_gray(search_area),prev_pts,maxCorners,qualityLevel,minDistance,Mat(),blockSize,useHarrisDetector,k );	


						//printf("Offset: %d, %d\n", search_area.x, search_area.y);
						//compute new core
						float sum_y = 0, sum_x = 0;

						for (int j = 0; j < prev_pts.size(); j++) {

							sum_x += prev_pts[j].x;
							sum_y += prev_pts[j].y;

							//circle(frame_gray, pt, 4, Scalar(255,255,255));

						}

						//printf("Min x: %d; Min y: %d; sum_x: %d; sum_y: %d\n", min_x, min_y, sum_x, sum_y);

						float core_x = (sum_x / prev_pts.size());
						float core_y = (sum_y / prev_pts.size());

						Point new_core = Point((int) (core_x + search_area.x), (int) (core_y + search_area.y));
						printf("New core is %d,%d\n", new_core.x, new_core.y);

						int x_shift = new_core.x - core.x;
 						int y_shift = new_core.y - core.y;

						int x_shift_abs = x_shift < 0 ? -x_shift : x_shift;
 						int y_shift_abs = y_shift < 0 ? -y_shift : y_shift;


						bool x_shift_out_of_scope = false;
						bool y_shift_out_of_scope = false;

						//ignore small, random shifts
						if (x_shift_abs > 3) {
							//printf("X shift of %d is above the lim of %.2f\n", x_shift, x_shift_upper_lim);
							//search_area.x += x_shift;
							x_shift_out_of_scope = true;
						}
						if (y_shift_abs > 3) {
							//printf("Y shift of %d is above the lim of %.2f\n", y_shift, y_shift_upper_lim);
							//search_area.y += y_shift;
							y_shift_out_of_scope = true;
						}

						//x below zero?
						if (search_area.x < 0) {
							search_area.x = 0;
						}
						//y below zero?
						if (search_area.y < 0) {
							search_area.y = 0;
						}

						//x overflow
						if (search_area.x + search_area.width > frame_gray.cols) {
							search_area.x = frame_gray.cols - search_area.width;
						}
						//y overflow
						if (search_area.y + search_area.height > frame_gray.rows) {
							search_area.y = frame_gray.rows - search_area.height;
						}

						//check if new off set is below 
						core = new_core;


						//break;

						if (prev_pts.size() < 40) {
							printf("Breaking out on too few points: %d\n", prev_pts.size());
							//break;
						}

						//find new points to look for
						//find core
						int face_core_height = (search_area.height / 18) * 17;
						int face_core_width  =  search_area.width / 2;
	

						face_core = Rect( search_area.x + face_core_width / 2, search_area.y + (face_core_height / 17), face_core_width, face_core_height);
						//minDistance = face_core_width / 5;
 
						prev_pts.clear();
						goodFeaturesToTrack( frame_gray(face_core),prev_pts,maxCorners,qualityLevel,minDistance,Mat(),blockSize,useHarrisDetector,k );



						if (cntr++ > 1000) break;
						if (cntr % 50 == 0) {
							imshow("Face 1", frame_gray(face_core));
						}
						frame_gray.copyTo(prev_frame);
						//prev_frame = frame;
					}

				waitKey(1);
			}
		}
	}

	capture.release();
	return 0;
}
