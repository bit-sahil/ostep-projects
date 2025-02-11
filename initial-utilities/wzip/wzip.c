#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {

	if(argc < 2) {
		printf("wzip: file1 [file2 ...]\n");
		exit(1);
	}

	FILE* fp;	

	int curr;
	int prev = -1, cnt = 0;

	for(int i = 1; i<argc; i++) {

		fp = fopen(argv[i], "r");
		if(fp == NULL) {
			printf("wzip: cannot open file\n");
			exit(1);
		}

		while((curr=fgetc(fp)) != EOF) {
			
			if(curr == prev) {
				cnt++;
			} else {
				// output
				if(prev != -1) {
					unsigned char bytes[5];

					bytes[3] = (cnt >> 24) & 0xFF;
					bytes[2] = (cnt >> 16) & 0xFF;
					bytes[1] = (cnt >> 8) & 0xFF;
					bytes[0] = cnt & 0xFF;
					bytes[4] = (int) prev;
					fwrite(&bytes, 1, 5, stdout);

					// printf("%d%c ", cnt, prev);
				}

				// reset state
				cnt = 1;
				prev = curr;
			}

			// printf("%c", curr);
		}

		fclose(fp);
	}

	
	if(prev != -1) {
		unsigned char bytes[5];

		bytes[3] = (cnt >> 24) & 0xFF;
		bytes[2] = (cnt >> 16) & 0xFF;
		bytes[1] = (cnt >> 8) & 0xFF;
		bytes[0] = cnt & 0xFF;
		bytes[4] = (int) prev;
		fwrite(&bytes, 1, 5, stdout);
		
		// printf("%d%c ", cnt, prev);
	}

	
	return 0;
}
