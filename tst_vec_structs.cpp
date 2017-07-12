#include<vector>
#include<stdlib.h>
#include<stdio.h>

using namespace std;

typedef struct {
long id;
int count;
} nn;

int main(int argc, char *argv[]) {

	vector<nn *> nns;

	nn *nn_1 = (nn *) malloc(sizeof(nn));

	nn_1->id = 0;
	nn_1->count = 0;

	nns.push_back(nn_1);

	nn *nn_2 = (nn *) malloc(sizeof(nn));

	nn_2->id = 1;
	nn_2->count = 1;

	nns.push_back(nn_2);

	for (int i = 0; i < nns.size(); i++) {
		printf("%d\n", nns[i]->id);
	}

	return 0;
}

