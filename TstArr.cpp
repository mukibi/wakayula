#include<stdio.h>

int main() {

	int size = 5;
	int array[size];


	for (int i = 0; i < size; i++) {
		array[i] = i * i;
	}

	for (int i = 0; i < size; i++) {
		printf("%d\n", array[i]);
	}
}
