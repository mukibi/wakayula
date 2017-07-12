#include<stdio.h>
#include <string>
#include <iostream>

#include <vector>

using namespace std;

int main() {

	string str("L 0,0");
	int offset = 0;

	vector<string> bts;

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

	if (bts[0] == "L") {
		printf("Line\n");

		for ( int i = 0; i < bts.size(); i++ ) {
			//cout << bts[i] << endl;
		}

		

	}

}

