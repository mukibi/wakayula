#include<cv.h>
#include<highgui.h>
#include<iostream>

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {

	double contrast = 2.0;
	int brightness = 50;

	Mat image = imread("/home/k0k1/Pictures/ikawa_contacts/20150724_110609.jpg");

	if ( image.data ) {
		Mat new_image = Mat::zeros(image.size(), image.type());

		for (int y = 0; y < image.rows; y++) {
			for (int x = 0; x < image.cols; x++)  {
				for (int channel = 0; channel < 3; channel++ ) {
					new_image.at<Vec3b>(y,x)[channel] = saturate_cast<uchar> (contrast * (image.at<Vec3b>(y,x)[channel]) + brightness);
				}
			}
		}

		//namedWindow("Original", WINDOW_AUTOSIZE);
		namedWindow("Altered", WINDOW_AUTOSIZE);

		//imshow("Original", image);
		imshow("Altered", new_image);
	}
	else {
		cout << "Could not read image" << endl;
	}

	waitKey(0);
	return 0;

}
