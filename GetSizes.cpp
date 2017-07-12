#include "opencv2/opencv.hpp"

#include<stdio.h>

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {

	int i;
	Mat img;

	for ( i = 1; i < argc; i++ ) {

		img = imread(argv[i]);
		if (img.data) {

			Size sz = img.size();
			printf("%s 1 0 0 %d %d\n", argv[i], sz.width, sz.height);
			
		}
	}
}


