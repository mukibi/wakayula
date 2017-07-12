#include<stdio.h>
#include<cv.h>
#include<highgui.h>

using namespace cv;

int main(int argc, char *argv[]) {

	Mat image,image_gray,dst_image;
	image = imread( "/home/k0k1/magumoni_day/pics/DSC03622.JPG" );
	if ( !image.data )
	{
		printf("No image data \n");
		return -1;
	}

	cvtColor(image,image_gray, CV_BGR2GRAY);
	threshold(image_gray, dst_image, 50, 255, 3);
	imshow("Camera", dst_image);
	waitKey(0);
}
