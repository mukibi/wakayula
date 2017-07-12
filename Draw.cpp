#include<cv.h>
#include<highgui.h>

using namespace cv;

int main(int argc, char *argv[]) {

	RNG rng(1989);

	int rand_num = (unsigned int) rng;

	Scalar color(rand_num & 255, (rand_num >> 8) & 255, (rand_num >> 16) & 255);
 
	Mat image = Mat::zeros(480, 600, CV_8UC3);

	Point pt1, pt2;

	pt1.x = 0;
	pt1.y = 0;

	pt2.x = 600;
	pt2.y = 480;

	line(image, pt1, pt2, color,10,CV_AA);

	Point pt3, pt4;

	pt3.x = 275;
	pt3.y = 215;
	
	pt4.x = 325;
	pt4.y = 265;
	
	rectangle(image, pt3, pt4, color, 5, CV_AA);
 
	Point pt5;

	pt5.x = 300;
	pt5.y = 240;

	Size axes;

	axes.width = 90;
	axes.height = 190;

	ellipse(image, pt4, axes,  360, 0, 360, color, 3, CV_AA);

	imshow("Drawing", image);
	waitKey(0);
	
}

