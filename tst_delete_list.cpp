#include<stdio.h>
#include<vector>

using namespace std;

int main() {

	vector<int> nums;

	for (int i = 0; i < 20; i++) {
		nums.push_back(i);
	}

	//delete the odd numbers
	for ( int i = 0; i < 20; i++ ) {
		if (i == 10) { 
			nums.erase(nums.begin()+0);
		}
	}

	for (int j = 0; j < nums.size(); j++) {
		printf("%d ", nums[j]);
	}
	printf("\n");


}

