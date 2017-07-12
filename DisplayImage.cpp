#include<stdio.h>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
//#include<opencv2/imgproc.hpp>

using namespace cv;

int main(int argc, char **argv) {

	Mat image,blurred,edges;

	image = imread( "/home/k0k1/Pictures/b4_net.png", CV_LOAD_IMAGE_GRAYSCALE );
	if ( !image.data )
	{
		printf("No image data \n");
		return -1;
	}

	GaussianBlur(image, blurred, Size(3,3), 3,3);
	Canny(blurred, edges, 0, 30, 3);
	//namedWindow("Original Image", WINDOW_AUTOSIZE );
	namedWindow("Blurred Image", WINDOW_AUTOSIZE );

	//imshow("Original Image", image);
	imshow("Blurred Image", blurred);
	imshow("Edges", edges);

	waitKey(0);

	return 0;
}
 
