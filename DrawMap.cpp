#include<stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include<vector>

using namespace cv;
using namespace std;

vector<string> polygon_names;
vector<vector<int> > polygons;

int width_cms  = 0;
int length_cms = 0;

int width_px  = 500;
int length_px = 500;

string sql_table = "map";

int read_shapes();
vector<string> get_bts(string);
int* get_nums(string);
int verify_data();
int draw_map();

int main() {

	read_shapes();

	if (verify_data() != 0) {
		return 0;
	}

	//draw_map
	draw_map();

	return(0);

}

int read_shapes() {

	//The file format of in.data is
	//name_of_the_polygon x,y x',y' x'',y''
	//to draw a polygon named 'name_of_the_polygon
	//with nodes
	//(x,y), (x',y') and (x'',y'')
	ifstream file("in.data");

	if (! file.is_open() ) {
		printf("Could not open 'in.data'\n");
		return 1;
	}

	string line;

	while ( getline(file, line) ) {

		vector<string> bts = get_bts(line);

		//Polygon
		vector<int> pts;

		for ( int i = 1; i < bts.size(); i++ ) {

			int *coords = get_nums(bts[i]);

			pts.push_back(coords[0]);
			pts.push_back(coords[1]);
	
		}
		//close polygon ring

		if (! ( (pts[0] == pts[pts.size() - 2]) && (pts[1] == pts[pts.size() - 1]) ) ) {

			pts.push_back(pts[0]);
			pts.push_back(pts[1]);

		}
		
		polygons.push_back(pts);
		polygon_names.push_back(bts[0]);

	}
	
	file.close();

	//write in SQl format
	ofstream file_out("out.sql", ios::trunc);

	if (! file_out.is_open() ) {
		printf("Could not open 'out.sql'\n");
		return 1;
	}

	//create table
	stringstream sql_statements;

	sql_statements << "CREATE TABLE ";
	sql_statements << sql_table;
	sql_statements << "(name varchar(32) unique primary key, geom geometry);\n";

	for (int i = 0; i < polygons.size(); i++) {

		sql_statements << "INSERT INTO ";
		sql_statements << sql_table;

		sql_statements << " VALUES('";
		sql_statements << polygon_names[i];
		sql_statements << "', ST_GeometryFromText('POLYGON((";

		int polygon_nodes = polygons[i].size();

		for ( int j = 0; j < polygon_nodes; j++ ) {

			sql_statements << polygons[i][j];
			//add comma
			if (j % 2 == 0) {
				sql_statements << " ";
			}
			
			//last coord
			else if (j != (polygon_nodes - 1) ) {
				sql_statements << ", ";
			}
		}

		sql_statements << "))', 0));\n";	
	}

	file_out << sql_statements.str();
	file_out.close();

	return 0;
}

vector<string> get_bts( const string str) {

	vector<string> bts;

	int offset = 0;

	while ( 1 ) {

		size_t found = str.find(" ", offset);

		//process the last elem
		if (found == string::npos) {

			string bt = str.substr(offset, found - offset);
			bts.push_back(bt);

			break;
		}
		else {

			string bt = str.substr(offset, found - offset);
			bts.push_back(bt);

			offset = found + 1;
		}
	}

	return bts;

}

int* get_nums(string str) {

	int *nums = (int *) calloc(2, sizeof(int));
 
	size_t found = str.find(",",0);

	if (found != string::npos) {
		string num_1 = str.substr(0, found);
		string num_2 = str.substr(found+1);

		nums[0] = atoi(num_1.c_str());
		nums[1] = atoi(num_2.c_str());
	}

	return nums;

}

int verify_data() {

	for ( int i = 0; i < polygons.size(); i++ ) {
		//each polygon
		//is an array of 2-dim points(x and y)
		//it must also have atleast 3 sets of points
		if ( ( ( polygons[i].size() % 2 ) != 0 ) || polygons[i].size() < 6 ) {
			printf("Invalid polygon at %d\n", i);
			return 1;
		}
	}

	return 0;
}


int draw_map() {

	Mat image = Mat::zeros(width_px, length_px, CV_8UC3);

	for ( int i = 0; i < polygons.size(); i++ ) {

		for ( int j = 0; j < polygons[i].size(); j+=2 ) {

			int pt_x = polygons[i][j];
			int pt_y = polygons[i][j+1];

			if ( pt_x > width_cms )  width_cms = pt_x;
			if ( pt_y > length_cms ) length_cms = pt_y;
		}
	}

	width_cms  += width_cms * 0.1;
	length_cms += length_cms * 0.1;
	
	//draw polygon
	for ( int i = 0; i < polygons.size(); i++ ) {

		vector<Point> pts;

		int mean_x = 0;
		int mean_y = 0;

		int cntr = 1;

		for ( int j = 0; j < polygons[i].size(); j+=2 ) {

			int pt_x = polygons[i][j];
			int pt_y = polygons[i][j+1];

			char buffer[50];
			sprintf(buffer, "%d,%d", pt_x, pt_y);

			string coords(buffer);

			//cout << coords << endl;

			int scaled_pt_x = ((float) pt_x  / (float)  width_cms) * (float) width_px;
			int scaled_pt_y = ((float) pt_y  / (float) length_cms) * (float) length_px;

			scaled_pt_y = length_px - scaled_pt_y;

			float delta_x = (scaled_pt_x - mean_x);
			float delta_y = (scaled_pt_y - mean_y);

			mean_x += delta_x / (float) (cntr);
			mean_y += delta_y / (float) (cntr);

			cntr++;

			Point pt(scaled_pt_x, scaled_pt_y);

			pts.push_back(pt);

			putText(image, coords, Point(scaled_pt_x + 5,scaled_pt_y - 5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255));

		}

		
		polylines(image, pts, true, Scalar(255, 255, 255));

		int txt_px = ( polygon_names[i].length()  * 3 );

		//printf("%s: %d x %d\n", polygon_names[i].c_str(), mean_x, mean_y);

		//drawMarker(image, Point(mean_x, mean_y), Scalar(255,255,255));

		//putText(image, polygon_names[i], Point(mean_x - txt_px, mean_y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255));
	}

	imshow("Map", image);
	waitKey(0);
	
	return(0);

}


