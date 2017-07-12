#include<iostream>
#include<string>
#include<sstream>

using namespace std;

int main() {

	stringstream ss;
	
	ss << 23 << " years";
	
	cout << ss.str() << endl;

}
