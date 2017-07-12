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


int main( int argc, char** argv ) {

	string file_n;
	ifstream data_file("file_list3.data");

	Mat image,resized;

	if (data_file.is_open()) {

		while ( getline(data_file, file_n) )  {

			image = imread(file_n);
			Size img_size = image.size();

			cout << file_n << ": " << img_size.height << "x" << img_size.width << endl;

		}

	}

}

