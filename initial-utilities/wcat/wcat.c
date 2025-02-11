#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {

	char buff[1024];
	FILE* fp;

	for(int i = 1; i<argc; i++) {

		fp = fopen(argv[i], "r");
		if(fp == NULL) {
			printf("wcat: cannot open file\n");
			exit(1);
		}


		while(fgets(buff, sizeof buff, fp) != NULL) {
			printf(buff);
		}

		fclose(fp);
	}
	
	return 0;
}
