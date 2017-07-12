#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include <string>
//#include <iostream>
#include<stdio.h>

using namespace cv;

int main(int argc, char *argv[]) {

	//printf("0\n");
	VideoCapture capture;
	capture.open(0);

	//printf("1\n");

	if (!capture.isOpened()) {
		printf("Error\n");
		return(0);
	}
	//printf("2\n");

	Mat view0;
	capture >> view0;

	Mat view;
	view0.copyTo(view);
	//printf("3\n");

	if (!view.data) {
		printf("No image captured");
		return(0);
	}
	//printf("4\n");

	namedWindow("Webcam", WINDOW_AUTOSIZE);
	//printf("5\n");

	imshow("Webcam", view); 
	//printf("6\n");

	waitKey(0);
	return(0);
}

