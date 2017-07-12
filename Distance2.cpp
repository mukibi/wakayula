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

int main() {

	capture.open(0);



	if ( capture.isOpened() ) {

		if ( face_cascade.load("/usr/local/share/OpenCV/lbpcascades/lbpcascade_frontalface.xml") ) {

			//if (eye_classifier.load("/usr/local/share/OpenCV/haarcascades/haarcascade_eye.xml")) {

				vector<int> focal_lens;
				vector<Rect> faces;

				for ( int i = 0; i < 100; i++ ) {
					faces.clear();	
					capture >> frame;



					Mat frame_gray;
					cvtColor( frame, frame_gray, CV_BGR2GRAY );
					equalizeHist( frame_gray, frame_gray );


					///-- Detect faces
					face_cascade.detectMultiScale( frame_gray, faces, 1.05, 6, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
					if (faces.size() == 0) continue;

					for (int j = 1; j < faces.size(); j++) {
						if (faces[j].width > faces[0].width) faces[0] = faces[j];
					}
						//Detect eyes
						//std::vector<Rect> eyes;

						//eye_classifier.detectMultiScale( frame_gray(faces[j]), eyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));
						//seen atleast 1 eye
						//if ( eyes.size() > 0 ) {
					int focal_len = (faces[0].width * 50) / 17;
					focal_lens.push_back(focal_len);
					imshow("Face", frame_gray(faces[0]));
					//break;

						//}
					//}

					waitKey(10);			
				}

				int sum = 0;
				for (int k = 0; k < focal_lens.size(); k++) {
					sum += focal_lens[k];
				}
				printf("Num faces: %d\n", focal_lens.size());
				int focal_len = sum / focal_lens.size();
				printf("%d\n", focal_len);
			//}
		}

	}
	else {
		printf("Could not open video device\n");
	}


	return 0;
}


void detect() {


}

