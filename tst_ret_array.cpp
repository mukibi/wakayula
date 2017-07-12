#include<stdio.h>
#include<stdlib.h>

int* ret_vals(int, int);

int main() {

	int *res = ret_vals(2, 3);	

	printf("%d; %d\n", res[0], res[1]);

}

int* ret_vals(int x, int y) {

	int *arr = (int *) calloc(2, sizeof(int));

	arr[0] = x * 2;
	arr[1] = y * 2;

	return arr;

}

