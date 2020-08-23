#define _GNU_SOURCE	/*for strcasestr function*/
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<string.h>
#include<dirent.h>
#include <regex.h> 
#include "mygrep.h"

/*Initialization of variables*/
void init() {
	
	flag_i = 0;
	flag_c = 0;
	flag_w = 0;
	flag_f = 0;
	flag_b = 0;
	flag_v = 0;
	flag_l = 0;
	flag_m = 0;
	flag_r = 0;
	flag_y = 0;
	flag_o = 0;
	flag_H = 0;
	flag_h = 0;
	flag_q = 0;
	flag_byte = 1; /*useful with -f option (mygrep_f() function)*/
	byte = -1;
	std_in = 0;
	qinit(&q);
}

/*print help.txt file*/
void help() {
	int fd;
	char ch;
	fd = open("files/help.txt", O_RDONLY);
	if(fd == -1) {
		perror("unable to open file");
	} 
	while(read(fd, &ch, 1)) {
		printf("%c", ch);
	}
	close(fd);
}

/*function to get all command line arguments*/
char* getArg(int argc, char *argv[]) {
	char option[50], *arg;
	int i = 0, opt;
	max = 0;
	while((opt = getopt(argc, argv, ":ilm:ryEe:oHhbcvwqf:x")) != -1)  
	{  
		switch(opt)  
		{  
			case 'i': 
				flag_i = 1;
				option[i++] = opt;  
				break;
			case 'm':
				flag_m = 1;
				option[i++] = opt;
				max = atoi(optarg);
				if(max <= 0)	/*grep works even if max is less than 0. This bug is removed*/
					printf("mygrep : Invalid max count\n"); 
				break;
			case 'H':
				if(flag_h == 0)
					flag_H = 1;
				option[i++] = opt;
				break;
			case 'w':
				flag_w = 1;
				option[i++] = opt;
				break;
			case 'v':
				flag_v = 1;
				option[i++] = opt;
				break;
			case 'b':  
				flag_b = 1;
				option[i++] = opt;
				break;
			case 'l':
				flag_l = 1;
				option[i++] = opt;
				break;
			case 'y':
				flag_y = 1;
				flag_i = 1;
				option[i++] = opt;
				break;
			case 'h':
				if(flag_H == 0)
					flag_h = 1;
				option[i++] = opt;
				break;
			case 'q': 
				flag_q = 1;
				option[i++] = opt;
				break;
			case 'r':
				flag_r = 1;
				option[i++] = opt;
				break;
			case 'o':
				flag_o = 1;
				option[i++] = opt;
				break;
			case 'E':
				flag_E = 1;
				option[i++] = opt;
				break;
			case 'c':   
				flag_c = 1;
				option[i++] = opt;
				break;  
			case 'f':  
				flag_f = 1;
				option[i++] = opt;
				std_in--;
				enq(&q, optarg);
				break;  
			case 'e':  
				flag_e = 1;
				option[i++] = opt; 
				std_in--;	//to maintain value of std_in
				enq(&q, optarg);
				break;  
			case ':':  
				printf("option needs a value\n");  
				break;  
			case '?':  
				printf("unknown option: %c\n", optopt); 
				break;
		}  
	}  
	option[i] = '\0';
	for(; optind < argc; optind++){ 
		std_in++;   
		enq(&q, argv[optind]);  /*enques all arguments after options*/
   	} 
	arg = option;
	return arg;	/*return string containg options*/
}

/*print version*/
void version() {
	int fd;
	char ch;
	fd = open("files/version.txt", O_RDONLY);
	if(fd == -1) {
		perror("unable to open file");
	} 
	while(read(fd, &ch, 1)) {
		printf("%c", ch);
	}
	close(fd);
}

//print usage
void usage() {
	int fd;
	char ch;
	fd = open("files/usage.txt", O_RDONLY);
	if(fd == -1) {
		perror("unable to open file");
	} 
	while(read(fd, &ch, 1)) {
		printf("%c", ch);
	}
	close(fd);
}

//print lines of file
void mygrep(char *pattern, char *file, queue *q) {
	int fd, res1 = 1, flag = 0, count = 0, no_of_lines = 0;
	char line[MAX], *token, line1[MAX], *res;
	if(opendir(file)) {
		printf("mygrep: %s: Is a directory\n", file);
		return;
	}
	if(!qempty(q))
		flag_H = 1;

	if((flag_m == 1) && max <= 0)
		return;
	do{
		byte = -1;
		fd = open(file, O_RDONLY);
		if(fd == -1) {
			perror("unable to open file");
			return;
		} 
		if(flag_m == 1)
			count = 1;// if flag_m is unset then count remains zero and does not have any effect on while condition
		while(strcpy(line, readline(fd, line)) && strcmp(line, "\0") && count <= max) {
			//if line not empty
			if(!(line[0] == '\n')) {		
				strcpy(line1, line);
				//if token is not null
				if((token = strtok(line, delimeter))) {
					do {	
						if(flag_w == 1 || flag_o == 1) {
							if(flag_i == 0 && flag_y == 0) {
								res1 = strcmp(token, pattern);	//if case sensitive
							}
							else if(flag_i == 1 || flag_y == 1) {
								res1 = strcasecmp(token, pattern);	//if case insensitive
							}
							if(!res1)
							{	
								if(flag_q == 1)
									exit(0);	//if match exit(0)
								flag = 1;
								if(flag_v == 0) {
									if(flag_c == 1) {
										no_of_lines++;
									}
									if(flag_m == 1)	
										count++;
									if(flag_c == 0) {
										if(flag_H == 1) {
											printf("\033[0;35m");
											printf("%s:", file);
											printf("\033[0m");
										}
										if(flag_b == 1) {
											printf("\033[0;32m");				
											printf("%lld:", byte);		
											printf("\033[0m");
										}		
										if(flag_o == 1) 
											printLine(pattern, token);
										else
											printLine(pattern, line1);
									}
								}
								break;	//break if match found
							}
						}
						else {
							//if flag_w == 0
							if(flag_i == 0 && flag_y == 0) {
								res = strstr(token, pattern);	//case sensitive
							}
							else if(flag_i == 1 || flag_y == 1) {
								res = strcasestr(token, pattern);	//case insensitive
							}
							if(res)
							{
								if(flag_q == 1)
									exit(0);
								flag = 1;
								if(flag_v == 0) {
									if(flag_c == 1) {
										no_of_lines++;
									}
									if(flag_m == 1)
										count++;
									if(flag_c == 0) {
										if(flag_H == 1) {
											printf("\033[0;35m");
											printf("%s:", file);
											printf("\033[0m");
										}
										if(flag_b == 1) {
											printf("\033[0;32m");					
											printf("%lld:", byte);		
											printf("\033[0m");
										}			
										if(flag_o == 1) 
											printLine(pattern, token);
										else
											printLine(pattern, line1);
									}
								}
								break;
							}
						}

					}while((token = strtok(NULL, delimeter)));
				}	//end of inner if
		
			}	//end of outer if

			if((flag_v == 1) && (flag_o == 0) && (flag == 0)) {
				if(flag_c == 1) {
					no_of_lines++;
				}
				if(flag_m == 1)
					count++;
				if(flag_c == 0) {
					if(flag_H == 1) {
						printf("\033[0;35m");
						printf("%s:", file);
						printf("\033[0m");
					}
					if(flag_b == 1) {
						printf("\033[0;32m");					
						printf("%lld:", byte);		
						printf("\033[0m");
					}
					printf("%s\n", line1);
					//printLine(pattern, line1);
				}
				line1[0] = '\0';
			}	
			flag = 0;
		}	//end of outer while

		close(fd);
		if(flag_c == 1) {
			if(flag_H == 1) {
				printf("\033[0;35m");
				printf("%s:", file);
				printf("\033[0m");
			}
			printf("%d\n", no_of_lines);
		}
		no_of_lines = 0;
	}while(!qempty(q) && (file = deq(q)));
}

//Function to read pattern form file and print accordingly
void mygrep_f(char *ptrn, char *file, queue *q) {
	int fd1, fd2, res1 = 1, flag = 0, count = 0, no_of_lines = 0;
	char line1[MAX], *token, pattern[MAX], line3[MAX], *res;
	if(opendir(file)) {
		printf("mygrep: %s: Is a directory\n", file);
		return;
	}

	if((flag_m == 1) && max <= 0)
		return;

	if(!qempty(q))
		flag_H = 1;
	do{
		byte = -1;
		fd2 = open(file, O_RDONLY);
		if(fd2 == -1) {
			perror("unable to open file");
			return;
		} 
		if(flag_m == 1)
			count = 1;// if flag_m is unset then count remains zero and does not have any effect on while condition
		while(strcpy(line1, readline(fd2, line1)) && strcmp(line1, "\0") && count <= max) {
			//if line is not empty
			if(!(line1[0] == '\n')) {		
				strcpy(line3, line1);
				if((token = strtok(line3, delimeter))) {
					do {
						flag_byte = 0;
						//opening file containing patterns
						fd1 = open(ptrn, O_RDONLY);
						if(fd1 == -1) {
							perror("unable to open file");
							return;
						}
						while(strcpy(pattern, readline(fd1, pattern)) && strcmp(pattern, "\0")) {
							if(!(pattern[0] == '\n')) {
								if(flag_w == 1 || flag_o == 1) {
									if(flag_i == 0 && flag_y == 0) {
										res1 = strcmp(token, pattern);	//case sensitive
									}
									else if(flag_i == 1 || flag_y == 1) {
										res1 = strcasecmp(token, pattern);	//case insensitive
									}
									if(!res1)
									{	
										if(flag_q == 1)
											exit(0);
										flag = 1;
										if(flag_v == 0) {
											if(flag_c == 1) {
												no_of_lines++;
											}
											if(flag_m == 1)
												count++;
											if(flag_c == 0) {
												if(flag_H == 1) {
													printf("\033[0;35m");
													printf("%s:", file);
													printf("\033[0m");
												}
												if(flag_b == 1) {
													printf("\033[0;32m");		
													printf("%lld:", byte);		
													printf("\033[0m");
												}			
												if(flag_o == 1) 
													printLine(pattern, token);
												else
													printLine(pattern, line1);
											}
										}
										break;
									}

								}
								else {
									//flag_w = 0
									if(flag_i == 0 && flag_y == 0) {
										res = strstr(token, pattern);	//case sensitive
									}
									else if(flag_i == 1 || flag_y == 1) {
										res = strcasestr(token, pattern);	//case insensitive
									}
									if(res)
									{
										if(flag_q == 1)
											exit(0);
										flag = 1;
										if(flag_v == 0) {
											if(flag_c == 1) {
												no_of_lines++;
											}
											if(flag_m == 1)
												count++;
											if(flag_c == 0) {
												if(flag_H == 1) {
													printf("\033[0;35m");
													printf("%s:", file);
													printf("\033[0m");
												}
												if(flag_b == 1) {
													printf("\033[0;32m");			
													printf("%lld:", byte);		
													printf("\033[0m");
												}			
												if(flag_o == 1) 
													printLine(pattern, token);
												else
													printLine(pattern, line1); //function prints line
											}
										}
										break;
									}
								}
							}	//inner if close
						}	//inner while close
						close(fd1);

					}while((token = strtok(NULL, delimeter)));
					flag_byte = 1;	//to restrict pattern file to change byte value
				}	//inner if close
			}	//outer if close
			if((flag_v == 1) && (flag_o == 0) && (flag == 0)) {
				if(flag_c == 1) {
					no_of_lines++;
				}
				if(flag_m == 1)
					count++;
				if(flag_c == 0) {
					if(flag_H == 1) {
						printf("\033[0;35m");
						printf("%s:", file);
						printf("\033[0m");
					}
					if(flag_b == 1) {
						printf("\033[0;32m");					
						printf("%lld:", byte);		
						printf("\033[0m");
					}
					printf("%s\n", line1);
					//printLine(pattern, line1);
				}
				line1[0] = '\0';
			}
			flag = 0;
		}	//outer while close
		close(fd2);
		if(flag_c == 1) {
			if(flag_H == 1) {
				printf("\033[0;35m");
				printf("%s:", file);
				printf("\033[0m");
			}
			printf("%d\n", no_of_lines);
		}
		no_of_lines = 0;
	} while(!qempty(q) && (file = deq(q)));

}

//Read line from file
char* readline(int fd, char *line) {
	int n=0, i=0;
	static int count = 0;
	char ch;
	if(flag_byte == 1) {
		if(byte == -1)
			count = 0;
		byte = count;
	}
	line[0] = '\0';
	while((n = read(fd, &ch, 1))){ 
		if(ch == '\n') {
			if(flag_byte == 1)
				count++;
			if(i == 0) {
				line[i++] = '\n';	//if empty line
			}
			
			line[i] = '\0';
			i = 0;
			return line;
		}
		else {
			if(flag_byte == 1)
				count++;
			line[i++] = ch;
		}
	}
	return line;
}

//Function for recursive search
void mygrep_r(char *pattern, char *dir) {
	int fd, fd1 = 0;	
	queue q1;
	char line[MAX];

	if((flag_m == 1) && max <= 0)	//if invalid max count
		return;

	if(flag_f == 1) {
		fd1 = open(pattern, O_RDONLY);
		if(fd1 == -1) {
			printf("grep: %s: No such file or directory\n", pattern);
			return;
		}
		close(fd1);
	}
		
	do {
		fd = open("temp.txt", O_CREAT | O_TRUNC);	//truncate temp.txt each time when new directory 
		close(fd);
		getFileFromDir(dir, pattern, &q1);
		fd = open("temp.txt", O_RDONLY);	//temp.txt contains names of all files and folders of given directory
		if(fd == -1) {
			perror("Unable to read directory");
			return;
		}
		while(strcpy(line, readline(fd, line)) && strcmp(line, "\0")) {
			if(opendir(line)) {
				//does nothing if directory
			}
			else{
				flag_H = 1;	//set flag_H to display file name along with lines
				qinit(&q1);
				if(flag_f == 1)
					mygrep_f(pattern, line, &q1);
				else
					mygrep(pattern, line, &q1);
			}
		}
		close(fd);
	}while(!qempty(&q) && (dir = deq(&q)));
}

//Function reads all directories, subdirectories and files from directory
void getFileFromDir(char *basePath, char* pattern, queue *q1)
{
    char path[MAX], path1[MAX];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
		strcpy(path1, path);
		strcat(path1, "\n");
		writeFile(path1);	//function to write path1 in file
            getFileFromDir(path, pattern, q1);
        }
    }

    closedir(dir);
}

//Function to write file names in temp.txt file
void writeFile(char *file) {
	int fd;
	fd = open("temp.txt", O_APPEND | O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("unable to open file");
		return;
	}
	if(!opendir(file)) { 
		write(fd, file, strlen(file));
	}
	close(fd);
}

//Function for regular expression
void regexMatch(char *pattern, char *file, queue *q) {
	int fd, res = 0, flag = 0, count = -1, no_of_lines = 0;
	char line[MAX], line1[MAX];
	if((flag_m == 1) && max <= 0)	//invalid max count
		return;
	if(opendir(file)) {
		printf("mygrep: %s: Is a directory\n", file);
		return;
	}
	if(!qempty(q))
		flag_H = 1;
	do{
		byte = -1;
		fd = open(file, O_RDONLY);
		if(fd == -1) {
			perror("unable to open file");
			return;
		} 
		if(flag_m == 1)
			count = 1;// if flag_m is unset then count remains zero and does not have any effect on while condition
		while(strcpy(line, readline(fd, line)) && strcmp(line, "\0") && count <= max) {
			//if not empty line
			if(!(line[0] == '\n')) {
					res = regexEval(pattern, line);	//regular expression evaluator function
					if(res)
					{	
						if(flag_q == 1)
							exit(0);
						flag = 1;
						if(flag_v == 0) {
							if(flag_c == 1) {
								no_of_lines++;
							}
							if(flag_m == 1)
								count++;
							if(flag_c == 0) {
								if(flag_H == 1) {
									printf("\033[0;35m");
									printf("%s:", file);
									printf("\033[0m");
								}
								if(flag_b == 1) {
									printf("\033[0;32m");					
									printf("%lld:", byte);		
									printf("\033[0m");
								}			
								printf("%s\n", line);
							}
						}
					}
			}
			if((flag_v == 1) && (flag_o == 0) && (flag == 0)) {
				if(flag_c == 1) {
					no_of_lines++;
				}
				if(flag_m == 1)
					count++;
				if(flag_c == 0) {
					if(flag_H == 1) {
						printf("\033[0;35m");
						printf("%s:", file);
						printf("\033[0m");
					}
					if(flag_b == 1) {
						printf("\033[0;32m");					
						printf("%lld:", byte);		
						printf("\033[0m");
					}
					printf("%s\n", line1);
				}
				line1[0] = '\0';
			}	//if close
			flag = 0;
		}	//while close
		close(fd);
		if(flag_c == 1) {
			if(flag_H == 1) {
				printf("\033[0;35m");
				printf("%s:", file);
				printf("\033[0m");
			}
			printf("%d\n", no_of_lines);
		}
		no_of_lines = 0;
	} while(!qempty(q) && (file = deq(q)));
}

//Function for evaluating regular expression
int regexEval(char *pattern, char *str) {
	regex_t regex;
	
	//compiling regular expression
	if(flag_i == 1 && (regcomp(&regex, pattern, REG_ICASE) == 0)) {
		//executing regular expression
		if(regexec(&regex, str, 0, NULL, 0) == 0) {		
			return 1;
		}
		else
			return 0;
	}
	else if(regcomp(&regex, pattern, REG_EXTENDED) == 0) {
		//executing regular expression
		if(regexec(&regex, str, 0, NULL, 0) == 0) {		
			return 1;
		}
		else
			return 0;
	}
	else {
		printf("mygrep: Invalid Regular Expression\n");
		exit(0);
	}

}

//function to print matched line with color effect
void printLine(char *pattern, char *line) {
	char *token;
	token = strtok(line, " \0");
	do {
		if(flag_w == 1) {
			if((flag_i == 1) && !strcasecmp(token, pattern)) {
				printf("\033[0;31m");
				printf("%s ", token);
				printf("\033[0m");
			}
			else if((flag_i == 0) && !strcmp(token, pattern)) {
				printf("\033[0;31m");
				printf("%s ", token);
				printf("\033[0m");
			}
			else
				printf("%s ", token);
		}
		else{
			if((flag_i == 1) && strcasestr(token, pattern)) {
				printf("\033[0;31m");
				printf("%s ", token);
				printf("\033[0m");
			}
			else if((flag_i == 0) && strstr(token, pattern)) {
				printf("\033[0;31m");
				printf("%s ", token);
				printf("\033[0m");
			}
			else
			printf("%s ", token);
		}

	}while((token = strtok(NULL, delimeter)));
	printf("\n");
}

//Pattern matching for standard input
void mygrep_stdin(char *pattern) {
	char c, line[1024];
	int i = 0;
	long long cnt = 0, byteOff, count = 0;
	byteOff = cnt;
	if((flag_m == 1) && (max <= 0))
		return;
	if(flag_m == 1)
		count = 1;
	while((c = getchar()) && (count <= max)) {
		if(c == '\n') {
			line[i] = '\0';
			cnt++;
			if(strcasestr(line, pattern)) {
				if(flag_H == 1) {
					printf("\033[0;35m");
					printf("(standard input):");
					printf("\033[0m");
				}
				if(flag_b == 1) {
					printf("\033[0;32m");					
					printf("%lld:", byteOff);		
					printf("\033[0m");
				}
					if(flag_m == 1)
						count++;
					printLine(pattern, line);
			}
			line[0] = '\0';
			i = 0;
			byteOff = cnt;
		}
		else {
			line[i++] = c;
			cnt++;
		}
	}
}

