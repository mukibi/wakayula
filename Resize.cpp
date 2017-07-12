#include <stdio.h>
#include <cv.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

using namespace std;
using namespace cv;


int main(int argc, char** argv) {

	string file_n;
	ifstream data_file("file_list2.data");

	int eigen_cntr = 0;
	Mat image,resized;
	if (data_file.is_open()) {

		while ( getline(data_file, file_n) )  {

			image = imread(file_n);
			resized = Mat::zeros(Size(50, 40), image.type());
			resize(image, resized, resized.size());

			string resized_file_n = "/home/k0k1/wakayula/faces2/facesdb/resized/";

			char file_id[4];
			sprintf(file_id, "%04d", eigen_cntr++);

			resized_file_n.append(file_id);
			resized_file_n.append(".png");
			
			imwrite(resized_file_n, resized);
		}
		data_file.close();
	}

/*
	Mat image = imread("/home/k0k1/Pictures/mt_kenya/DSC03275.JPG");

	if ( image.data ) {
		Mat resized = Mat::zeros(Size(75, 100), image.type());
		resize(image, resized, Size(75, 100));
		imwrite("resized.png", resized);
	}
*/
	return 0;
}

