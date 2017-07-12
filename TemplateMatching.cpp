#include<stdio.h>

#include<cv.h>
#include<highgui.h>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<time.h>

using namespace cv;

int main(int argc, char *argv[] ) {

	clock_t ticks_before = clock();

	Mat img_src,img_template,img_display,result;
	
	img_src = imread("template_matching_src.png");
	if (!img_src.data) {
		printf("Could not read src image\n");
		return 0;
	}

	img_template = imread("template_matching_template_5.png");
	if (!img_template.data) {
		printf("Could not read src image\n");
		return 0;
	}

	img_src.copyTo(img_display);
	
	int result_cols = img_src.cols - img_template.cols + 1;
	int result_rows = img_src.rows - img_template.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	int match_type = CV_TM_SQDIFF;

	clock_t ticks_after = clock();
	printf("Ticks: %d\n", ticks_after - ticks_before);

	matchTemplate(img_src, img_template, result, match_type);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal, maxVal;
	Point minLoc, maxLoc, matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc); 

	if (match_type == CV_TM_SQDIFF || match_type == CV_TM_SQDIFF_NORMED) {
		matchLoc = minLoc;
	}
	else {
		matchLoc = maxLoc;
	}

	rectangle(img_display, matchLoc, Point(matchLoc.x + img_template.cols, matchLoc.y + img_template.rows), Scalar::all(0), 2, 8, 0);
	rectangle(result, matchLoc, Point(matchLoc.x + img_template.cols, matchLoc.y + img_template.rows), Scalar::all(0), 2, 8, 0);

	ticks_after = clock();
	printf("Ticks: %d\n", ticks_after - ticks_before);

	namedWindow("Image Window", CV_WINDOW_NORMAL);
	namedWindow("Result Window", CV_WINDOW_NORMAL);

	imshow("Image Window", img_display);
	imshow("Result Window", result);

	waitKey(0);
}

