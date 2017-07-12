#include<cv.h>
#include<highgui.h>
#include<stdlib.h>

using namespace cv;

int main(int argc, char *argv[]) {

	double alpha = 0.1;
	int beta = 20;

	if (argc > 2) {
		alpha = atof(argv[1]);
		beta = atol(argv[2]);	
	}

	Mat original_image = imread("/home/k0k1/Pictures/ikawa_contacts/20150724_110609.jpg", CV_LOAD_IMAGE_COLOR);
	Mat new_image = Mat::zeros(original_image.size(), original_image.type());

	if (original_image.data) {
		original_image.convertTo(new_image, -1, alpha, beta );
		imwrite("/home/k0k1/Pictures/altered.jpg", new_image);
	}
	
	waitKey(0);
	return 0;
}
