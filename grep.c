/*-r  -i  -v  -f  -w  -c  -m  -b -q  -H  -h */

/*main file*/
#define _GNU_SOURCE /*for strcasestr function*/
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>  
#include<dirent.h>
#include "mygrep.c"
int subStr(char *str1, char *str2) {
	int len1 = 0, len2 = 0, cnt = 0, temp, temp1;
	len1 = strlen(str1);
	len2 = strlen(str2);
	temp1 = len1;
	while(temp1 > 0) {
		temp1--;
		temp = len2;
		while(temp > 0) {
			temp--;
			if(str1[temp1] == str2[temp]) {
				cnt++;
				break;
			}
		}
	}
	if(len1 == cnt)
		return 1;
	else
		return 0;
}
int main(int argc, char *argv[]) {
	char *option, *file, *pattern;
	init();
	if(argc == 1) {
		usage();
	}
	if(argc == 2) {
		if(!strcmp(argv[1], "--help") || !strcmp(argv[1], "--h")) {
			help();
		}
		if(!strcmp(argv[1], "-V") || !strcmp(argv[1], "--version")) {
			version();
		}
		else {
			
			mygrep_stdin(argv[1]);
		}
	}
	else {
		option = getArg(argc, argv);
		if(std_in == 1) {
			pattern = deq(&q);
			mygrep_stdin(argv[1]);
		}
		else if(subStr(option, "iobwHyhvmc")) {
			pattern = deq(&q);
			file = deq(&q);
			mygrep(pattern, file, &q);	/*compare with normal options*/
		}
		else if(subStr(option, "iwvHhymcbof")) {
			pattern = deq(&q);	
			file = deq(&q);
			mygrep_f(pattern, file, &q);	/*compare with normal options + pattern from file*/
		}
		else if(subStr(option, "wbvifcbHhymor")) {
			pattern = deq(&q);
			file = deq(&q);
			mygrep_r(pattern, file);	/*compare with normal options + directory*/
		}
		else if(subStr(option, "cviwHmbeE")) {
			pattern = deq(&q);
			file = deq(&q);
			regexMatch(pattern, file, &q);	/*compare with normal options + regular expression*/
		}
		
	}

	return 0;

}
