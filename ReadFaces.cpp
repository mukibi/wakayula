#include<stdio.h>
#include<stdlib.h>
#include<time.h>

//#include "opencv2/video/tracking.hpp"
#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"

#include <mysql.h>
#include <my_global.h>
#include <my_sys.h>
#include <string.h>


using namespace cv;
using namespace std;

int init_db();
int store_face(int, int, int, int, int, int, int);

MYSQL *con;
MYSQL_STMT *stmt;
MYSQL_BIND bind_params[7];

int main ( int argc, char *argv[] ) {

	if (argc < 3) {
		printf("Usage: %s filename max_face_size_px\n", argv[0]);
		return 0;
	}

	string f_name(argv[1]);

	int max_face_size = atoi(argv[2]);

	if ( ! max_face_size > 0 ) {
		printf("Invalid max_face_size_px provided\n");
		return 0;
	}

	int db_initd = init_db();
	if (!db_initd) {
		printf("Could not open DB\n");
		return 0;
	}
	
	//return 0;
	int maxCorners = 100;
	double qualityLevel = 0.01;
	double minDistance = 1;
  	int blockSize = 3;
  	bool useHarrisDetector = false;
 	double k = 0.04;

	CascadeClassifier face_cascade;
	vector<Rect> faces;
	vector<Point2f> points;

	if ( face_cascade.load("/usr/share/OpenCV/lbpcascades/lbpcascade_frontalface.xml") ) {

		VideoCapture capture;
		int frame_cntr = -1;
		capture.open(f_name);
		//int mean_ticks = 0;
		//clock_t ticks_before, ticks_after;

		if ( capture.isOpened() ) {

			Mat frame,frame_gray;

			while ( true ) {
			
				capture >> frame;
				if (frame.empty()) break;
			
				frame_cntr++;
				cvtColor( frame, frame_gray, CV_BGR2GRAY );
				equalizeHist( frame_gray, frame_gray );


				faces.clear();
				face_cascade.detectMultiScale( frame_gray, faces, 1.05, 6, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30), Size(max_face_size, max_face_size) );

				if ( faces.size() > 0 )  {	

					for ( int i = 0; i < faces.size(); i++ ) {

						int face_rect_height = faces[i].height * 3;

						if ( (face_rect_height + faces[i].y) > frame_gray.rows )  {
							face_rect_height = frame_gray.rows - faces[i].y;
						}
						faces[i].height = face_rect_height;
	
						faces[i].x += (faces[i].width / 6);
						faces[i].width *= (2.0/3.0);
			
						points.clear();
						goodFeaturesToTrack( frame_gray(faces[i]),points,maxCorners,qualityLevel,minDistance,Mat(),blockSize,useHarrisDetector,k );

						float mean_x=0.0, mean_y = 0.0;

						for (int j = 0; j < points.size(); j++) {

							mean_x += (points[j].x - mean_x) / (float) (j+1);
							mean_y += (points[j].y - mean_y) / (float) (j+1);
						}
						
						store_face(frame_cntr, faces[i].x, faces[i].y, faces[i].width, faces[i].height, (int) mean_x, (int) mean_y);
					}
				}
			}

			capture.release();
			mysql_commit(con);
			mysql_close(con);

		}

		else {
			printf("Could not open camera\n");
		}

	}
	else {
		printf("Could not load LBP Cascade\n");
	}

	return 0;
}

int init_db() {

	//make db connection
	con = mysql_init(NULL);
	con = mysql_real_connect(con, "localhost", "wakayula_user", "w4k4w4k4", "wakayula_db", 0, NULL, 0);
	mysql_autocommit(con, 0);
	if (con == NULL) {
		printf("Connection error: %s\n", mysql_error(con));
		return 0;
	}

	//prepare statement
	stmt = mysql_stmt_init(con);

						 //face,frame,x, y, w, h,c_x,c_y
	char *stmt_str = (char *) "INSERT INTO `faces` VALUES(NULL, ?, ?, ?, ?, ?, ?, ?)";
	long stmt_str_len = strlen(stmt_str);

	int rc = mysql_stmt_prepare(stmt, stmt_str, stmt_str_len);

	if (rc != 0) {
		printf("Could not prepare INSERT INTO FACES: %s\n", mysql_stmt_error(stmt));
		return 0;
	}

	return 1;
}

int store_face(int frame, int x, int y, int width, int height, int core_x, int core_y) {

	memset(bind_params, 0, sizeof(bind_params));

	//frame	
	bind_params[0].buffer_type = MYSQL_TYPE_LONG;
	bind_params[0].buffer = (char *) &frame;
	bind_params[0].is_null = 0;
	bind_params[0].length = 0;

	//x
	bind_params[1].buffer_type = MYSQL_TYPE_LONG;
	bind_params[1].buffer = (char *) &x;
	bind_params[1].is_null = 0;
	bind_params[1].length = 0;


	//y
	bind_params[2].buffer_type = MYSQL_TYPE_LONG;
	bind_params[2].buffer = (char *) &y;
	bind_params[2].is_null = 0;
	bind_params[2].length = 0;

	
	//width
	bind_params[3].buffer_type = MYSQL_TYPE_LONG;
	bind_params[3].buffer = (char *) &width;
	bind_params[3].is_null = 0;
	bind_params[3].length = 0;


	//height
	bind_params[4].buffer_type = MYSQL_TYPE_LONG;
	bind_params[4].buffer = (char *) &height;
	bind_params[4].is_null = 0;
	bind_params[4].length = 0;


	//core_x
	bind_params[5].buffer_type = MYSQL_TYPE_LONG;
	bind_params[5].buffer = (char *) &core_x;
	bind_params[5].is_null = 0;
	bind_params[5].length = 0;


	//core_y
	bind_params[6].buffer_type = MYSQL_TYPE_LONG;
	bind_params[6].buffer = (char *) &core_y;
	bind_params[6].is_null = 0;
	bind_params[6].length = 0;


	my_bool rc_bool = mysql_stmt_bind_param(stmt, bind_params);

	if (rc_bool != 0) {
		printf("Could not bind params: %s\n", mysql_stmt_error(stmt));
		return 0;
	}

	
	int rc = mysql_stmt_execute(stmt);

	if (rc != 0) {
		printf("Could not execute INSERT INTO `faces`: %s\n", mysql_stmt_error(stmt));
		return 0;
	}

	return 1;
}
