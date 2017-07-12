#include<cv.h>
#include<opencv2/core/core.hpp>
#include<stdio.h>

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {
	
	RNG rng(1989);
	
	int num = (unsigned int) rng;

	int color = (num & 255);	
	cout << color << endl;

	int color_2 = ( (num >> 8) & 255);	
	cout << color_2 << endl;

	return 0;
}


