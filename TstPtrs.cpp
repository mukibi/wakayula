#include "opencv2/core.hpp"
#include<iostream>

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {

	Ptr<string> name(new string("Kenneth Njeru"));

	cout << *name << endl;
	
}

