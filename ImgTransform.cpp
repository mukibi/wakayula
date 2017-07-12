#include<cv.h>
#include<highgui.h>

using namespace cv;

int main(int argc, char *argv[]) {

	Mat image;

	image = imread("/home/k0k1/Pictures/b4_net.png");

	if (image.data) {
		Mat gray_image;
		cvtColor(image, gray_image, CV_BGR2GRAY);
		imwrite("/home/k0k1/Pictures/b4_net_gray.png", gray_image);
	}

	waitKey(0);
	return 0;

	
}

