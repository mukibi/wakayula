
#include "opencv2/video/tracking.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"

using namespace std;
using namespace cv;

int main(int argc, char **argv) {

	VideoWriter writer;

	if ( !writer.open("fisi_benson.mp4", 0x21, 25, Size(277, 394)) ) {

		printf("Could not open VideoWriter\n");
		return -1;

	}	

	Mat fisi1, fisi2, dst;

	fisi1 = imread("/home/k0k1/Pictures/fisi/fisi_one.png");
	fisi2 = imread("/home/k0k1/Pictures/fisi/fisi_two.png");

	float alpha = 0.0;
	float beta  = 1.0;

	for (int i = 0; i < 11; i++) {

		alpha = i * 0.1;
		beta  = 1 - alpha;

		addWeighted(fisi1, alpha, fisi2, beta, 0.0, dst);

		int plus = 0;
		if (i == 0 || i == 10) plus = 10;

		for ( int j = 0; j < (10+plus); j++ ) {
			writer.write(dst);
		}

		//imshow("Fisi", dst);
		//waitKey(1000);
	}

	writer.release();
	return 0;

}
