#include<stdio.h>
#include<stdlib.h>

typedef struct {

int a;
int b;

} num;

num *get_num();

int main() {
	
	num *a_num = get_num();
	printf("%d: %d\n", a_num->a, a_num->b);
	num *a_num2 = a_num;
	free(a_num2);
	free(a_num);
}

num *get_num() {

	num *a_num = (num *) malloc(sizeof(num));

	a_num->a = 1;
	a_num->b = 2;

	return a_num;
}
