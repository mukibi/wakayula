#include<stdio.h>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

Mat frame;
CascadeClassifier face_cascade;
CascadeClassifier eye_classifier;

VideoCapture capture;
void detect();

int main(int argc, char *argv[]) {

	capture.open(0);
	namedWindow("Face");

	if ( face_cascade.load("/usr/local/share/OpenCV/haarcascades/haarcascade_frontal_face.xml") ) {
		//if (eye_classifier.load(
		if ( capture.isOpened()) {
			while(true) {
				detect();
				waitKey(2000);
			}
		}
	}
	else {
		printf("Could not load cascade classifier\n");
	}

	return 0;
}

void detect() {


	printf("..");	

	capture >> frame;

	std::vector<Rect> faces;

	Mat frame_gray;
	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	///-- Detect faces
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

	int size = 0;
	for ( size_t i = 0; i < faces.size(); i++ ) {

		rectangle(frame_gray, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), Scalar(0,0,0), 3);
		printf("Rectangle width: %d\n", faces[i].width);
		size++;

	}

	if (size) {
		imshow("Face", frame_gray);
		//waitKey(0);
		//break;
	}
	printf("Detected %d faces\n", size);

}

