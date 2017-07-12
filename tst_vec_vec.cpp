#include<stdio.h>
#include<vector>
#include<math.h>

using namespace std;

int main() {

	vector< vector<int> > squares;

	for (int i = 0; i < 11; i++) {
		vector<int> row;

		for (int j = 0; j < 3; j++) { 
			int powd = (int) powf((float) i+2, (float) j+2);
			row.push_back(powd);
			//squares[i].push_back(powd);
		}
		squares.push_back(row);
	}

	for (int i = 0; i < squares.size(); i++) {
		printf("%d: ", i+2);
		for (int j = 0; j < squares[i].size(); j++) {
			printf("%d ", squares[i][j]);
		}
		printf("\n");
	}
}

