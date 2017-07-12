#include<stdio.h>
#include<vector>

using namespace std;

int main() {
	vector<int> last5_nums;

	for (int i = 0; i < 20; i++) {

		last5_nums.push_back(i);

		if (last5_nums.size() > 5) {
			last5_nums.erase(last5_nums.begin());
		}

	}

	for (int j = 0; j < last5_nums.size(); j++) {
		printf("%d ", last5_nums[j]);
	}
	printf("\n");

	return 0;
}
