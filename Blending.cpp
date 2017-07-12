#include<cv.h>
#include<highgui.h>

using namespace cv;

int main(int argc, char *argv[]) {
	Mat src1, src2, dst;
	double alpha = 0.5, beta = 0.5;

	src1 = imread("/home/k0k1/Pictures/ikawa_contacts/20150724_110609.jpg");
	src2 = imread("/home/k0k1/Pictures/ikawa_contacts/20150724_110737.jpg");
	
	if (src1.data and src2.data) {
		namedWindow("Linear blend", 1);
		addWeighted(src1, alpha, src2, beta, 0.0, dst);
		if (dst.data) {
			imshow("Linear blend", dst);
		}
	}

	waitKey(0);
	return 0;
}
