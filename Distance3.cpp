#include<stdio.h>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int main() {

	Mat frame;
	CascadeClassifier face_cascade;
	CascadeClassifier eye_classifier;

	VideoCapture capture;

	capture.open(0);


	if ( capture.isOpened() ) {

		if ( face_cascade.load("/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml") ) {

			if (eye_classifier.load("/usr/local/share/OpenCV/haarcascades/haarcascade_eye.xml")) {

				for ( int i = 0; i < 100; i++ ) {
	
					capture >> frame;

					std::vector<Rect> faces;

					Mat frame_gray;
					cvtColor( frame, frame_gray, CV_BGR2GRAY );
					equalizeHist( frame_gray, frame_gray );


					///-- Detect faces
					face_cascade.detectMultiScale( frame_gray, faces, 1.05, 6, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30), Size() );
	
					for (int j = 0; j < faces.size(); j++) {

						//Detect eyes
						std::vector<Rect> eyes;

						eye_classifier.detectMultiScale( frame_gray(faces[j]), eyes, 1.05, 3, 0|CV_HAAR_SCALE_IMAGE, Size(5,5), Size());
						imshow("Face", frame_gray(faces[j]));
						//seen atleast 1 eye
						if ( eyes.size() > 0 ) {

							//imshow("Face", frame_gray(eyes[1]));
							int pixel_width = faces[j].width;
							int distance = (600 * 15) /  faces[j].width;
							printf("Distance: %d\n", distance);	

						}
					}

					waitKey(1);
				}
			}
		}
		capture.release();
	}

}

