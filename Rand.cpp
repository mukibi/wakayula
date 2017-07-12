#include<cv.h>
#include<highgui.h>

using namespace cv;

int main(int argc, char *argv[]) {

	RNG rng(1989);
	Mat image = Mat::zeros(480, 600, CV_8UC3);

	int rand_num = (unsigned int) rng;
	Scalar color(rand_num & 255, (rand_num >> 8) & 255, (rand_num >> 16) & 255);

	Point pt;
	pt.x = 0;
	pt.y = 300;

	putText(image, "This is a test string",pt, 1, 5, color);
	imshow("Drawing", image);
	waitKey(0);


}
