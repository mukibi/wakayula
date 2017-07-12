#include<stdlib.h>
#include<stdio.h>
#include<vector>

using namespace std;

typedef struct {

int id;
int rev_id;

} face;



int main() {

	vector<face *> faces;

	for ( int i = 0; i < 10; i++ ) {

		face *a_face = (face *) malloc(sizeof(face));

		a_face->id = i;
		a_face->rev_id = 9-i;

		faces.push_back(a_face);

	}

	for (int i = 0; i < faces.size(); i++) {
		printf("%d: %d\n", faces[i]->id, faces[i]->rev_id);
	}

}

