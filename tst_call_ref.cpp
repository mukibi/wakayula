#include<stdio.h>
#include<vector>
#include<time.h>

using namespace std;

int sum(const vector<int>& val);
int sum2(const vector<int> val);

int main() {

	vector<int> *my_vec = new vector<int>();
	vector<int> my_vec2;
	
	for ( int i = 0; i < 10001; i++ ) {
		(*my_vec).push_back(i);
		my_vec2.push_back(i);
	}

	vector<int> *vec_ptr = &my_vec2;
	printf("%d\n", (*vec_ptr)[90]);

	clock_t ticks_before = clock();
	sum(*my_vec);	
	clock_t ticks_after = clock();

	printf("Ref Num ticks: %d\n", (ticks_after - ticks_before));

	ticks_before = clock();
	sum(my_vec2);	
	ticks_after = clock();

	printf("Val Num ticks: %d\n", (ticks_after - ticks_before));
/*
	printf("Num ticks: %d\n", (ticks_after - ticks_before));
	//delete vec_ptr;

	printf("Origi size: %d\n", my_vec2.size());
	//printf("%d\n", sum2(my_vec2));
	//printf("%d\n", sum(*my_vec));
	//delete my_vec;
*/
}

int sum(const vector<int> &vec) {

	int sum = 0;

	for (int i = 0; i < vec.size(); i++) {
		sum += vec[i];
	}

	return sum;

}

int sum2(const vector<int> vec) {

	int sum = 0;

	for (int i = 0; i < vec.size(); i++) {
		sum += vec[i];
	}
	return sum;
}
