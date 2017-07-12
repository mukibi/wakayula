#include<stdio.h>

#include<cv.h>
#include<highgui.h>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace cv;

Mat src,src_gray;
Mat dst,detected_edges;


int edge_threshold = 1;
int low_threshold;
int const max_lowThreshold = 200;
int ratio = 2;
int kernel_size = 3;


void UpdateThreshold(int, void *) {

	blur(src_gray, detected_edges, Size(3,3));
	Canny(detected_edges, detected_edges, low_threshold, low_threshold * ratio, kernel_size);
	dst = Scalar::all(0);

	src.copyTo(dst, detected_edges);
	imshow("Test Edges", dst);

}

int main(int argc, char *argv[]) {

	src = imread( "/home/k0k1/Pictures/mt_kenya/DSC03320.JPG" );

	if ( !src.data ) {
		printf("Could not read image\n");
		return 0;
	}

	dst.create(src.size(), src.type());
	cvtColor(src, src_gray, CV_BGR2GRAY);
	
	namedWindow("Test Edges", CV_WINDOW_NORMAL);
	createTrackbar("Min Threshold: ", "Test Edges", &low_threshold, max_lowThreshold, UpdateThreshold);

	UpdateThreshold(0, 0);

	waitKey(0);
	return 0;
}

