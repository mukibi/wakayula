#include <typeinfo>
#include<stdio.h>

#include "opencv2/video/tracking.hpp"
#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"

#include <mysql.h>
#include <my_global.h>
#include <my_sys.h>
#include <string.h>

#include<map>
#include<vector>


using namespace cv;
using namespace std;


typedef struct {

int face_id;
int frame;
int x;
int y;
int width;
int height;
int core_x;
int core_y; 

} face;

typedef struct {

int core_x;
int core_y;
int radius;
int distance;
int offset;
int id;

} face_location;

typedef struct {

int distance_cms;
int x_shift_cms;

} map_locus;

typedef struct {

int init_radius;
int mean_x;
int mean_y;

} face_radius;
 
int init_db();
int get_next_frame(int);
int get_next_faces(int);
int register_face(int, int);
face_radius *calculate_radius(const vector<Point2f>);
int calculate_diagonal(map_locus *, map_locus *);
int log_point(int, int, int, int, double);
int calculate_pixel_diagonal(int, int, int, int);
int *get_absolute_position(int, int);

MYSQL *con;

MYSQL_STMT *get_next_frame_stmt;
MYSQL_STMT *get_next_faces_stmt;
MYSQL_STMT *register_face_stmt;
MYSQL_STMT *log_point_stmt;

MYSQL_BIND get_next_frame_params[1];
MYSQL_BIND get_next_faces_params[1];
MYSQL_BIND  register_face_params[1];
MYSQL_BIND  	log_point_params[4];

MYSQL_BIND get_next_frame_results[1];
MYSQL_BIND get_next_faces_results[7];

vector<face *> next_faces;

int job_id = -1;
int focal_length = -1;
int frames_per_sec = -1;
int frame_zero_start = -1;
int max_failed_frames = 2;

int camera_angle = 0;
int camera_pos_x = 0;
int camera_pos_y = 0;

//Usage: TrackMotion <file> <camera focal length> <camera frames per sec> <frame zero start> <camera angle> <camera position(x)> <camera position(y)> 

int main(int argc, char *argv[]) {	

	int maxCorners = 100;
	double qualityLevel = 0.01;
	double minDistance = 1;
	int blockSize = 3;
	bool useHarrisDetector = false;
	double k = 0.04;


	int frame_centre = -1;

	if (argc < 9) return 0;
	string f_name(argv[1]);

	job_id = atoi(argv[2]);

	focal_length = atoi(argv[3]);
	if (!focal_length > 0) return 0;
	
	frames_per_sec = atoi(argv[4]);
	if (!frames_per_sec > 0) return 0;

	frame_zero_start = atoi(argv[5]);
	if (!frame_zero_start > 0) return 0;

	double last_face_time = frame_zero_start;

	camera_angle = atoi(argv[6]);
	if (camera_angle < 0 || camera_angle > 360) return 0;

	camera_pos_x = atoi(argv[7]);
	if (camera_pos_x < 0) return 0;

	camera_pos_y = atoi(argv[8]);
	if (camera_pos_y < 0) return 0;

	//allow a maximum lapse of 200ms
	//between frames during optical flow
	//analysis
	max_failed_frames = (int) frames_per_sec / 5;
	max_failed_frames += 1;

	int db_initd = init_db();	
	if (!db_initd) {
		printf("Could not open DB\n");
		return 0;
	}

	int next_frame = get_next_frame(-1l);	

	VideoCapture capture;
	int frame_cntr = -1;
	capture.open(f_name);
	
	vector<Mat> unresolved_frames;

	vector< vector<Point2f> > prev_points;
	vector<Point2f> next_points;
	vector<uchar> status;
	vector<float> err;
	vector<Point2f> tmp_prev_points;

	vector<face_location *> face_locations;
	//vector<map_locus *> tracked_map_locations;

	//map<int, int> failed_frames;

	if ( capture.isOpened() ) {


		Mat frame,frame_gray,prev_frame;

		while ( true ) {
	
			capture >> frame;
			if (frame.empty()) break;

			if (frame_centre == -1) {
				frame_centre = frame.cols / 2;
			}

			cvtColor( frame, frame_gray, CV_BGR2GRAY );
			equalizeHist( frame_gray, frame_gray );

			frame_cntr++;	

			//recalculate the keypoints
			//for each face in this frame
			//find the closest matching face in memory
			//and update the keypoints for that face

			if ( frame_cntr == next_frame ) {	
	
				get_next_faces(next_frame);

				//dont want to use the same face multiple times
				int taken_points[prev_points.size()];

				for (int i = 0; i < prev_points.size(); i++) {
					taken_points[i] = 0;
				}

				double time = (double) frame_zero_start + (double) ((double) frame_cntr/ (double) frames_per_sec);

				for ( int j = 0; j < next_faces.size(); j++ ) {

					//key points
					tmp_prev_points.clear();
					Rect face(next_faces[j]->x, next_faces[j]->y, next_faces[j]->width, next_faces[j]->height);

					goodFeaturesToTrack( frame_gray( face ),tmp_prev_points,maxCorners,qualityLevel,minDistance,Mat(),blockSize,useHarrisDetector,k );

					//make coords universal
					for ( int i = 0; i < tmp_prev_points.size(); i++ ) {
						tmp_prev_points[i].x += face.x;
						tmp_prev_points[i].y += face.y;
					}

					int matched = 0;


					face_radius *rad = calculate_radius(tmp_prev_points);

					for (int i = 0; i < prev_points.size(); i++) {

						//don't want to use any face multiple times
						if (taken_points[i] != 0) {
							continue;
						}

						status.clear();
						err.clear();
						next_points.clear();

						//compare the points in next_points with tmp_prev_points
						//a 67% match indicates we are looking at the same face

						calcOpticalFlowPyrLK(prev_frame, frame_gray, prev_points[i], next_points, status, err);
	
						matched = 0;

						for ( int m = 0; m < status.size(); m++ ) {
							if (status[m] == 1) matched++;
						}


						//Looking for a match rate of 67%
						//so accept 80% from the first run(calcOpticalFlowPyrLK)
						//and 80% for the second run(comparison of points)
						float match_ratio = matched / (float) prev_points[i].size();

						if ( match_ratio > 0.67 ) {

							//draw tmp_prev_points
							face_radius *rad2 = calculate_radius(next_points);

							int circle_shift = calculate_pixel_diagonal(rad->mean_x, rad->mean_y, rad2->mean_x, rad2->mean_y);
							int radius = rad->init_radius;
							//use the larger radius to delimit
							//the bounds of the search area
							if ( rad2->init_radius > radius ) {
								radius = rad2->init_radius;
							}

							free(rad2);


							if ( circle_shift <= (2*radius) ) {

								//log movement if significant
								//find initial location
								map_locus *init_loc = (map_locus *) malloc(sizeof(map_locus));

								init_loc->distance_cms = face_locations[i]->distance;
								init_loc->x_shift_cms =  face_locations[i]->offset;
		
								//find new location
								int new_distance = (10 * focal_length ) / next_faces[j]->width;

								int new_offset_pixels = rad->mean_x - frame_centre;
								int new_offset_cms = (new_offset_pixels * new_distance) / focal_length;

								map_locus *new_loc = (map_locus *) malloc(sizeof(map_locus));
								
								new_loc->distance_cms = new_distance;
								new_loc->x_shift_cms  = new_offset_cms;

								int distance_moved = calculate_diagonal(init_loc, new_loc);

								//if moved more than 40cms, log movement
								if ( distance_moved > 40 ) {

									face_locations[i]->radius = rad->init_radius;
									face_locations[i]->core_x = rad->mean_x;
									face_locations[i]->core_y = rad->mean_y;
									face_locations[i]->offset = new_offset_cms;
									face_locations[i]->distance = new_distance;

									int *pos = get_absolute_position( face_locations[i]->offset, face_locations[i]->distance);
									log_point( job_id, face_locations[j]->id, pos[1], pos[0], time );
									
								}

								free(init_loc);
								free(new_loc);

								//update points
								prev_points[i] = tmp_prev_points;			
								taken_points[i]++;
								matched++;
								break;
							}

						}

					}



					//register face
					//add points to list
					if ( !matched ) {	
	
						register_face(job_id, next_faces[j]->face_id);
						prev_points.push_back(tmp_prev_points);
				
						//store in face_locations
						face_location *face_loc = (face_location *) malloc(sizeof (face_location));
	

						face_loc->radius = rad->init_radius;
						face_loc->core_x = rad->mean_x;
						face_loc->core_y = rad->mean_y;

						//printf("Radius is %d\n", radius);
						//calculate distance to image(in cms)
						//using the formula:
						//D = (WxF)/P
						//use a W of 10cm because the width
						//stored is 2/3 of the width of the face
						//i.e. 2/3 of 16cm = 10cm
						int distance = (10 * focal_length ) / next_faces[j]->width;

						int offset_pixels = rad->mean_x - frame_centre;
						int offset_cms = (offset_pixels * distance) / focal_length;

						face_loc->distance = distance;
						face_loc->offset = offset_cms;
						face_loc->id = next_faces[j]->face_id;

						face_locations.push_back(face_loc);

						int *pos = get_absolute_position( face_loc->offset, face_loc->distance);

						log_point( job_id, next_faces[j]->face_id, pos[1], pos[0], time );
					}

					free(rad);
				}

				next_frame = get_next_frame(next_frame);

			}

			//simply track the face(s) using
			//the current known keypoints
			//logging any significant mvt (>0.4m)
			else {

				for ( int i = 0; i < prev_points.size(); i++ ) {

					status.clear();
					err.clear();
					next_points.clear();
					//check if this set of points is one of the failed ones	

					calcOpticalFlowPyrLK(prev_frame, frame_gray, prev_points[i] , next_points, status, err);

					int matched = 0;

					for ( int j = 0; j < status.size(); j++ ) {
						if (status[j] == 1) matched++;
					}


					//if match ratio is 67% plus, pass
					float match_ratio = matched / (float) prev_points[i].size();

					if ( match_ratio > 0.67 ) {
		
						prev_points[i] = next_points;
						//printf("%d: %d of %d: successfully tracked\n", frame_cntr, i, prev_points.size());

					}
					//if could not do a flow on the points
					//increment the failure count of this frame
					else {
						//always check failed_frames from index 1, not index zero
						//this increment will cause the value of unknown keys to be
						//1
						//failed_frames[i]++;

						//if more than <max_failed_frames> tried
						//delete from prev_points, face_locations and tracked_map_locations
					

						prev_points.erase(prev_points.begin()+i);

						   //free this pointer first
						free(face_locations[i]);
						face_locations[i] = NULL;
						face_locations.erase(face_locations.begin()+i);
				
					}

				}	

			}

			frame_gray.copyTo(prev_frame);	
		}
	
		capture.release() ;	
		mysql_commit(con);
		mysql_close(con);

	}
	else {
		printf("Could not open camera\n");
	}

}

int init_db() {

	//make db connection
	con = mysql_init(NULL);
	con = mysql_real_connect(con, "localhost", "wakayula_user", "w4k4w4k4", "wakayula_db", 0, NULL, 0);
	
	if (con == NULL) {
		printf("Connection error: %s\n", mysql_error(con));
		return 0;
	}
	mysql_autocommit(con, 0);

	//get next frame
	get_next_frame_stmt = mysql_stmt_init(con);

						 //face,frame,x, y, w, h,c_x,c_y
	char *get_next_frame_str = (char *) "SELECT frame FROM `faces` WHERE frame>? ORDER BY frame ASC LIMIT 1";
	int get_next_frame_str_len = strlen(get_next_frame_str);

	int rc = mysql_stmt_prepare(get_next_frame_stmt, get_next_frame_str, get_next_frame_str_len);

	if (rc != 0) {
		printf("Could not prepare SELECT FROM `faces`: %s\n", mysql_stmt_error(get_next_frame_stmt));
		return 0;
	}

	//get next face
	get_next_faces_stmt = mysql_stmt_init(con);

						 //face,frame,x, y, w, h,c_x,c_y
	char *get_next_faces_str = (char *) "SELECT face_id, x, y, width, height, core_x, core_y FROM `faces` WHERE frame=?";
	int get_next_faces_str_len = strlen(get_next_faces_str);

	rc = mysql_stmt_prepare(get_next_faces_stmt, get_next_faces_str, get_next_faces_str_len);

	if (rc != 0) {
		printf("Could not prepare SELECT FROM `faces`: %s\n", mysql_stmt_error(get_next_faces_stmt));
		return 0;
	}

	register_face_stmt = mysql_stmt_init(con);
								//face_id, face_id_alias
	char *register_face_str = (char *) "REPLACE INTO `face_alias` VALUES(?, '')";
	int register_face_str_len = strlen(register_face_str);

	rc = mysql_stmt_prepare(register_face_stmt, register_face_str, register_face_str_len);

	if (rc != 0) {
		printf("Could not prepare INSERT INTO `face_alias`: %s\n", mysql_stmt_error(register_face_stmt));
		return 0;
	}

	log_point_stmt = mysql_stmt_init(con);

	char *log_point_str = (char *) "INSERT INTO `face_movement` VALUES(NULL, ?, ?, ?, ?)";
	int log_point_str_len = strlen(log_point_str);

	rc =  mysql_stmt_prepare(log_point_stmt, log_point_str, log_point_str_len);

	if (rc != 0) {
		printf("Could not prepare INSERT INTO `face_movement`: %s\n", mysql_stmt_error(log_point_stmt));
		return 0;
	}

	return 1;

}

int get_next_frame(int current_loc) {
/*
	int nums[] = {9,28,29,31,32,34,37,38,41,42,44,46,51,52,53,54,55,57,60,64,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,90,91,92,93,94,95,96,97,98,99,100,101};
	for (int i = 0; i < sizeof(nums)/sizeof(int); i++) {
		if (i > current_loc) {
			return nums[i];
		}
	}
	return -1;
*/
	memset(get_next_frame_params, 0, sizeof(get_next_frame_params));

	get_next_frame_params[0].buffer_type = MYSQL_TYPE_LONG;
	get_next_frame_params[0].buffer = (char *) &current_loc;
	get_next_frame_params[0].is_null = 0;
	get_next_frame_params[0].length = 0;

	my_bool rc_bool = mysql_stmt_bind_param(get_next_frame_stmt, get_next_frame_params);

	if (rc_bool != 0) {
		printf("Could not bind params: %s\n", mysql_stmt_error(get_next_frame_stmt));
		return -1;
	}

	memset(get_next_frame_results, 0, sizeof(get_next_frame_results));

	int next_frame = -1;

	get_next_frame_results[0].buffer_type = MYSQL_TYPE_LONG;
	get_next_frame_results[0].buffer = (char *) &next_frame;
	get_next_frame_results[0].is_null = 0;
	get_next_frame_results[0].length = 0;

	rc_bool = mysql_stmt_bind_result(get_next_frame_stmt, get_next_frame_results);

	if (rc_bool != 0) {
		printf("Could not bind result: %s\n", mysql_stmt_error(get_next_frame_stmt));
		return -1;
	}	

	int rc = mysql_stmt_execute(get_next_frame_stmt);

	if (rc != 0) {
		printf("Could not execute SELECT FROM `faces`: %s\n", mysql_stmt_error(get_next_frame_stmt));
		return -1;
	}
	
	while (1) {

		rc = mysql_stmt_fetch(get_next_frame_stmt);

		if (rc == MYSQL_NO_DATA ) {
			break;
		}

	}

	return next_frame;

}

int get_next_faces(int frame_i) {

	memset(get_next_faces_params, 0, sizeof(get_next_faces_params));

	get_next_faces_params[0].buffer_type = MYSQL_TYPE_LONG;
	get_next_faces_params[0].buffer = (char *) &frame_i;
	get_next_faces_params[0].is_null = 0;
	get_next_faces_params[0].length = 0;

	my_bool rc_bool = mysql_stmt_bind_param(get_next_faces_stmt, get_next_faces_params);

	if (rc_bool != 0) {
		printf("Could not bind params: %s\n", mysql_stmt_error(get_next_faces_stmt));
		return -1;
	}

	memset(get_next_faces_results, 0, sizeof(get_next_faces_results));

	int face_id = -1;
	int x = -1;
	int y = -1;
	int width = -1;
	int height = -1;
	int core_x = -1;
	int core_y = -1;

	//face_id
	get_next_faces_results[0].buffer_type = MYSQL_TYPE_LONG;
	get_next_faces_results[0].buffer = (char *) &face_id;
	get_next_faces_results[0].is_null = 0;
	get_next_faces_results[0].length = 0;
	
	//x
	get_next_faces_results[1].buffer_type = MYSQL_TYPE_LONG;
	get_next_faces_results[1].buffer = (char *) &x;
	get_next_faces_results[1].is_null = 0;
	get_next_faces_results[1].length = 0;


	//y
	get_next_faces_results[2].buffer_type = MYSQL_TYPE_LONG;
	get_next_faces_results[2].buffer = (char *) &y;
	get_next_faces_results[2].is_null = 0;
	get_next_faces_results[2].length = 0;

	
	//width
	get_next_faces_results[3].buffer_type = MYSQL_TYPE_LONG;
	get_next_faces_results[3].buffer = (char *) &width;
	get_next_faces_results[3].is_null = 0;
	get_next_faces_results[3].length = 0;


	//height
	get_next_faces_results[4].buffer_type = MYSQL_TYPE_LONG;
	get_next_faces_results[4].buffer = (char *) &height;
	get_next_faces_results[4].is_null = 0;
	get_next_faces_results[4].length = 0;


	//core_x
	get_next_faces_results[5].buffer_type = MYSQL_TYPE_LONG;
	get_next_faces_results[5].buffer = (char *) &core_x;
	get_next_faces_results[5].is_null = 0;
	get_next_faces_results[5].length = 0;


	//core_y
	get_next_faces_results[6].buffer_type = MYSQL_TYPE_LONG;
	get_next_faces_results[6].buffer = (char *) &core_y;
	get_next_faces_results[6].is_null = 0;
	get_next_faces_results[6].length = 0;

	rc_bool = mysql_stmt_bind_result(get_next_faces_stmt, get_next_faces_results);

	if (rc_bool != 0) {
		printf("Could not bind result: %s\n", mysql_stmt_error(get_next_faces_stmt));
		return -1;
	}

	int rc = mysql_stmt_execute(get_next_faces_stmt);

	if (rc != 0) {
		printf("Could not execute SELECT FROM `faces`: %s\n", mysql_stmt_error(get_next_faces_stmt));
		return -1;
	}

	//free up all faces in the
	//next_faces vector
	for ( int i = 0; i < next_faces.size(); i++ ) {
		free(next_faces[i]);
		next_faces[i] = NULL;
	}

	next_faces.clear();

	while (1) {

		int rc = mysql_stmt_fetch(get_next_faces_stmt);

		if (rc == 0) {
			face *face_seen = (face *) malloc(sizeof (face));

			face_seen->face_id = face_id;
			face_seen->frame = frame_i;
			face_seen->x = x;
			face_seen->y = y;
			face_seen->width = width;
			face_seen->height = height;
			face_seen->core_x = core_x;
			face_seen->core_y = core_y;

			next_faces.push_back(face_seen);
		}

		else if (rc == MYSQL_NO_DATA) break;

	}

	return 1;
}

int register_face(int job_id, int face_id) {

	//face_id_str has a maximum of 15 characters
	char *face_id_str_ptr = (char *) malloc(16);
	snprintf(face_id_str_ptr, 16, "%d_%d", job_id, face_id);


	memset(register_face_params, 0, sizeof(register_face_params));

	register_face_params[0].buffer_type = MYSQL_TYPE_STRING;
	register_face_params[0].buffer = face_id_str_ptr;
	register_face_params[0].buffer_length = strlen(face_id_str_ptr);
	register_face_params[0].is_null = 0;

	my_bool rc_bool = mysql_stmt_bind_param(register_face_stmt, register_face_params);

	if (rc_bool != 0) {
		printf("Could not bind params: %s\n", mysql_stmt_error(register_face_stmt));
		return -1;
	}

	int rc = mysql_stmt_execute(register_face_stmt);

	if (rc != 0) {
		printf("Could not execute INSERT INTO `face_alias`: %s\n", mysql_stmt_error(register_face_stmt));
		return -1;
	}

	return 1;

}

//log a point in time and space(cartesian plane)
int log_point(int job_id, int face_id, int distance, int x_shift, double time) {

	char *face_id_str_ptr = (char *) malloc(16);
	snprintf(face_id_str_ptr, 16, "%d_%d", job_id, face_id);

	memset(log_point_params, 0, sizeof(log_point_params));

	//face_id
	log_point_params[0].buffer_type = MYSQL_TYPE_STRING;
	log_point_params[0].buffer = face_id_str_ptr;
	log_point_params[0].buffer_length = strlen(face_id_str_ptr);
	log_point_params[0].is_null = 0;

	//distance
	log_point_params[1].buffer_type = MYSQL_TYPE_LONG;
	log_point_params[1].buffer = (char *) &distance;
	log_point_params[1].is_null = 0;
	log_point_params[1].length = 0;

	//x-shift
	log_point_params[2].buffer_type = MYSQL_TYPE_LONG;
	log_point_params[2].buffer = (char *) &x_shift;
	log_point_params[2].is_null = 0;
	log_point_params[2].length = 0;

	//time
	log_point_params[3].buffer_type = MYSQL_TYPE_DOUBLE;
	log_point_params[3].buffer = (char *) &time;
	log_point_params[3].is_null = 0;
	log_point_params[3].length = 0;

	my_bool rc_bool = mysql_stmt_bind_param(log_point_stmt, log_point_params);

	if (rc_bool != 0) {
		printf("Could not bind params: %s\n", mysql_stmt_error(log_point_stmt));
		return -1;
	}

	int rc = mysql_stmt_execute(log_point_stmt);

	if (rc != 0) {
		printf("Could not execute INSERT INTO `face_movement`: %s\n", mysql_stmt_error(log_point_stmt));
		return -1;
	}

	return 1;

}
//caclculate the radius of an ROI
//i.e the mean distance of all points
//within 1 standard deviation of the core
//(center)
face_radius *calculate_radius(const vector<Point2f> points) {

	//calculate core
	float mean_x=0.0, mean_y = 0.0;
	float m2_x = 0.0, m2_y = 0.0;

	//printf("Size: %d\n", points.size());
	for ( int i = 0; i < points.size(); i++ ) {

		float delta_x = (points[i].x - mean_x);
		float delta_y = (points[i].y - mean_y);

		mean_x += delta_x / (float) (i+1);
		mean_y += delta_y / (float) (i+1);

		m2_x += (delta_x * (points[i].x - mean_x));
		m2_y += (delta_y * (points[i].y - mean_y));

	}

	//printf("Mean x: %f; Mean y: %f\n", mean_x, mean_y);

	float n_dev_x = sqrtf(m2_x / (points.size() - 1));
	float n_dev_y = sqrtf(m2_y / (points.size() - 1));

	//printf("Dev x: %f; Dev y: %f\n", n_dev_x, n_dev_y);

	//Calculate radius
	float init_radius = 0.0;

	//int exclude_x = 0, exclude_y = 0;

	for ( int i = 0; i < points.size(); i++ ) {

		float delta_x = fabsf(mean_x - points[i].x); 
		float delta_y = fabsf(mean_y - points[i].y); 

		if (delta_x > (n_dev_x * 2) ) continue;
		if (delta_y > (n_dev_y * 2) ) continue;

		float delta = sqrtf(powf(delta_x, 2) + powf(delta_y, 2));

		init_radius += (delta - init_radius) / (float) (i+1);

	}

	//printf("Exclude x: %d; exclude y: %d\n", exclude_x, exclude_y);

	face_radius *res = (face_radius *) malloc(sizeof(face_radius)); 
	
	res->init_radius = (int) init_radius;
	res->mean_x = (int) mean_x;
	res->mean_y = (int) mean_y;

	return res;
}

int calculate_diagonal(map_locus *pt1, map_locus *pt2) {


	int x_delta = abs(pt1->x_shift_cms  - pt2->x_shift_cms);
	int y_delta = abs(pt1->distance_cms - pt2->distance_cms);

	int diagonal = (int) sqrtf(powf(x_delta, 2) + powf(y_delta, 2));

	return diagonal;
}

int calculate_pixel_diagonal(int pt1_x, int pt1_y, int pt2_x, int pt2_y) {

	int x_delta = abs(pt1_x - pt2_x);
	int y_delta = abs(pt1_y - pt2_y);

	int diagonal = (int) sqrtf(powf(x_delta, 2) + powf(y_delta, 2));

	return diagonal;

}

int *get_absolute_position(int x_shift, int distance) {

	int new_angle = camera_angle;

	float rads = asinf( (float) abs(x_shift) / (float) distance );
	float degs = rads*57.3065;
	
	//if on the right, add
	if (x_shift > 0) {
		new_angle += degs;
	}
	else {
		new_angle -= degs;
	}

	int nearest_line = new_angle / 90;
	float nearest_line_float = (float) new_angle / 90.0f;

	int _360_deg = 0;

	if ( (nearest_line_float - nearest_line) >= 0.5 ) {
		nearest_line++;
	}

	if (nearest_line > 3) {
		nearest_line = 0;
		_360_deg++;
	}


	//use abs() to handle situations
	//where the point is to the left
	//of 0 degs
	int rem_angle_unabs = (nearest_line * 90) - new_angle; 
	int rem_angle = abs(rem_angle_unabs);

	if (_360_deg) {
		rem_angle = abs(360 - new_angle); 
	}

	int x_shift_2 =  distance * sin(rem_angle*0.01745);
	int distance_2 = distance * cos(rem_angle*0.01745);

	//figure out if the point is above/below / right/left 
	//int left = 0;
	//int below = 0;

	int new_x = camera_pos_x;
	int new_y = camera_pos_y;

	if ( nearest_line == 1 ) {

		//below
		if (rem_angle_unabs < 0) {
			new_x += distance_2;
			new_y -= x_shift_2;	
		}
		//above
		else {
			new_x += distance_2;
			new_y += x_shift_2;
		}

	}

	else if ( nearest_line == 2 ) {

		//left
		if (rem_angle_unabs < 0) {
			new_x -= x_shift_2;
			new_y -= distance_2;	
		}
		//right
		else {
			new_x += x_shift_2;
			new_y -= distance_2;
		}

	}

	else if (nearest_line == 3) {

		//below
		if ( rem_angle_unabs >= 0 ) {
			new_x -= distance_2;
			new_y -= x_shift_2;
		}
		//above
		else {
			new_x -= distance_2;
			new_y += x_shift_2;
		}

	}

	//left
	else if (_360_deg) {
		new_x -= x_shift_2;
		new_y += distance_2;
	}

	//right
	else if (nearest_line == 0) {

		if ( x_shift < 0 ) {
			new_x -= x_shift_2;
			new_y += distance_2;
		}
		else {
			new_x += x_shift_2;
			new_y += distance_2;
		}
	}


	int *abs_position = (int *) calloc(2, sizeof(int));

	abs_position[0] = new_x;
	abs_position[1] = new_y;

	return abs_position;
}

