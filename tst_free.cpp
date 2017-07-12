#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include<vector>
using namespace std;

void get_size();
typedef struct{

int value;
int index;

} num;

int main() {

	vector<num *> nums;

	for (int i = 0; i <10000; i++) {

		num *a_num = (num *) malloc(sizeof(num));

		a_num->value = i;
		a_num->index = i;

		nums.push_back(a_num);
	}

	get_size();
/*
	int num_dels = 0;

	for (int i = 0; i < 10000; i++) {
		if (i % 3 == 0) {
			//free(nums[i-num_dels]);
			nums.erase(nums.begin()+(i-num_dels));
			num_dels++;

			//printf("%d ", i);
		}
	}
*/
	nums.clear();
	vector<num *> empty(0);

	nums.swap(empty);
	//printf("%d\n", nums.size());
	//free(nums);
	get_size();
	//sleep(5);
}

void get_size() {

	pid_t pid = getpid();
	
	int file_path_len = strlen("/proc/xxxxxx/statm");
	char *file_path = (char *) malloc(file_path_len);

	snprintf(file_path, file_path_len, "%s%d%s", "/proc/", pid, "/statm");

	int fd = open(file_path, O_RDONLY);

	if (fd == -1) {
		printf("Error on open: %s\n", strerror(errno));
	}

	char buffer[64];

	//read file
	int n_bytes = read(fd, buffer, 64);

	char *str = (char *) malloc(n_bytes-1);
	strncpy(str, buffer, n_bytes-1);

	printf("+%s+\n", str);

	close(fd);

}
