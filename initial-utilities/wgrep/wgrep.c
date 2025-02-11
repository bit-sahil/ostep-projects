#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]) {

	if(argc < 2) {
		printf("wgrep: searchterm [file ...]\n");
		exit(1);
	}

	char* st = argv[1];
	FILE* fp;

	for(int i = 2; i<argc; i++) {

		fp = fopen(argv[i], "r");
		if(fp == NULL) {
			printf("wgrep: cannot open file\n");
			exit(1);
		}

		char *buffer = NULL;
	    size_t bufsize = 32;

		while(getline(&buffer,&bufsize,fp) > 0) {
			char *s = strstr(buffer, st);
			if(s!=NULL)
				printf("%s", buffer);
		}

		fclose(fp);
	}
	
	if(argc == 2) {
		// when no input files are provided, read from standard input instead
		char *buffer = NULL;
	    size_t bufsize = 32;

		while(getline(&buffer,&bufsize,stdin) > 0) {
			char *s = strstr(buffer, st);
			if(s!=NULL)
				printf("%s", buffer);
		}
	}

	return 0;
}
