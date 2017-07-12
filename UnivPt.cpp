//#include <typeinfo>
#include<stdio.h>
#include<math.h>

//#include "opencv2/video/tracking.hpp"
//#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"


using namespace cv;
using namespace std;

int main(int arc, char *argv[]) {

	Mat img;


	int distance = 100;
	int x_shift = -50;

	for (int angle = 45; angle < 360; angle += 90) {

		img = Mat::zeros(300, 300, CV_32F);

		/*if (angle == 0) {
			line(img, Point(150, 150), Point(150, 50), Scalar(255, 255, 255));
		}*/

		if (angle == 45) {
			line(img, Point(150, 150), Point(220, 80), Scalar(255, 255, 255));
		}

		/*else if (angle == 90) {
			line(img, Point(150, 150), Point(250, 150), Scalar(255, 255, 255));
		}*/

		else if (angle == 135) {
			line(img, Point(150, 150), Point(220, 220), Scalar(255, 255, 255));
		}

		/*else if (angle == 180) {
			line(img, Point(150, 150), Point(150, 250), Scalar(255, 255, 255));
		}*/

		else if (angle == 225) {
			line(img, Point(150, 150), Point(80, 220), Scalar(255, 255, 255));
		}

		/*else if (angle == 270) {
			line(img, Point(150, 150), Point(50, 150), Scalar(255, 255, 255));	
		}*/

		else  if (angle == 315) {
			line(img, Point(150, 150), Point(80, 80), Scalar(255, 255, 255));	
		}


		int new_angle = angle;

		float rads = asinf( (float) abs(x_shift) / (float) distance );
		float degs = rads*57.3065;

		
		//if on the right, add
		if (x_shift > 0) {
			new_angle += degs;
		}
		else {
			new_angle -= degs;
		}

		int nearest_line = new_angle / 90;
		float nearest_line_float = (float) new_angle / 90.0f;

		int _360_deg = 0;

		if ( (nearest_line_float - nearest_line) >= 0.5 ) {
			nearest_line++;
		}

		if (nearest_line > 3) {
			nearest_line = 0;
			_360_deg++;
		}


		//use abs() to handle situations
		//where the point is to the left
		//of 0 degs
		int rem_angle_unabs = (nearest_line * 90) - new_angle; 
		int rem_angle = abs(rem_angle_unabs);

		if (_360_deg) {
			rem_angle = abs(360 - new_angle); 
		}

		int x_shift_2 =  distance * sin(rem_angle*0.01745);
		int distance_2 = distance * cos(rem_angle*0.01745);

		/*if (nearest_line == 0 || nearest_line == 2) {
			int dup_x_shift_2 = x_shift_2;
			x_shift_2 = distance_2;
			distance_2 = dup_x_shift_2;
		}*/

		//figure out if the point is above/below / right/left 
		//int left = 0;
		//int below = 0;

		int new_x = 150;
		int new_y = 150;

		if ( nearest_line == 1 ) {

			//below
			if (rem_angle_unabs < 0) {
				new_x += distance_2;
				new_y += x_shift_2;	
			}
			//above
			else {
				new_x += distance_2;
				new_y -= x_shift_2;
			}

		}

		else if ( nearest_line == 2 ) {

			//left
			if (rem_angle_unabs < 0) {
				new_x -= x_shift_2;
				new_y += distance_2;	
			}
			//right
			else {
				new_x += x_shift_2;
				new_y += distance_2;
			}

		}

		else if (nearest_line == 3) {

			//below
			if ( rem_angle_unabs >= 0 ) {
				new_x -= distance_2;
				new_y += x_shift_2;
			}
			//above
			else {
				new_x -= distance_2;
				new_y -= x_shift_2;
			}

		}

		//left
		else if (_360_deg) {
			new_x -= x_shift_2;
			new_y -= distance_2;
		}

		//right
		else if (nearest_line == 0) {	
			new_x += x_shift_2;
			new_y -= distance_2;
		}

		printf( "%d: %d : (%d,%d)\n", angle, nearest_line, x_shift_2, distance_2 );

		circle(img, Point(new_x, new_y), 4, Scalar(255, 255, 255));
		
		imshow("Img", img);
		waitKey(2000);
	

	}


	return 0;
}
