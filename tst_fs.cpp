#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

using namespace cv;

int main(int argc, char *argv[]) {

	FileStorage fs("tst.xml", FileStorage::WRITE);
	
	fs << "name" << string("Kenneth Njeru");

	fs << "nom_de_plume" << "[";
	fs << string("k0k1");
	fs << string("Njogu");
	fs << string("Kiao");
	fs << "]";

	fs.release();
	return(0);
}

