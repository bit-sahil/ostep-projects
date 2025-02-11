#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {

	if(argc < 2) {
		printf("wunzip: file1 [file2 ...]\n");
		exit(1);
	}	

	FILE* fp;

	for(int i=1; i<argc; i++) {
		fp = fopen(argv[i], "r");
		if(fp == NULL) {
			printf("wunzip: cannot open file\n");
			exit(1);
		}

		unsigned char bytes[5];
		long cnt = 0;
		char c;

		while(fread(&bytes, 1, 5, fp) == 5) {
			c = (char) bytes[4];
			
			// cnt = bytes[0] + (bytes[1]  << 8) + (bytes[2] << 16) + (bytes[3] << 24);
			cnt = bytes[0];
			cnt = cnt | (((unsigned long) bytes[1]) << 8);
			cnt = cnt | (((unsigned long) bytes[2]) << 16);
			cnt = cnt | (((unsigned long) bytes[3]) << 24);

			for (int i=0; i<cnt; i++) {
				putc(c, stdout);
			}
		}
			
		fclose(fp);
	}
	
	
	return 0;
}
