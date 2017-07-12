//#include "opencv2/core.hpp"
#include<stdio.h>
#include<stdlib.h>

//using namespace cv;

int main ( int argc, char *argv[] ) {
	int *ptr = (int *) malloc(sizeof(int));
	
	*ptr = 23;

	int *dup_ptr = ptr;

	//free(dup_ptr);
	
	printf("%d\n", *ptr);
	
}

