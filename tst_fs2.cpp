#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

using namespace cv;

int main(int argc, char *argv[]) {

	FileStorage fs("tst.xml", FileStorage::READ);

	if (!fs.isOpened()) {
		return(0);
	}

	FileNode fn = fs.getFirstTopLevelNode();

	if (!fn.type() == FileNode::SEQ) {
		return(0);
	}

	FileNodeIterator it_end = fn.end();

	for ( FileNodeIterator it = fn.begin(); it != it_end; it++ ) {
		cout << (string) *it << endl;
	}

}

