#include "queue.c"
#define MAX 20480
int flag_i;
int flag_c;
int flag_w;
int flag_f;
int flag_b;
int flag_v;
int flag_l;
int flag_m;
int flag_r;
int flag_y;
int flag_o;
int flag_H;
int flag_h;
int flag_q;
int flag_E;
int flag_e;
int flag_byte;	//useful in -f
int max;	//max with -m option
int std_in;	//For standard input
long long byte;	//for byteset
char delimeter[] = " '<>()[]{}^/|+=-%$#@!~`&\t.*,?;:\n\0";	//delimeters for tokenization
void init();	//initialize variables
void help();	//print help
void version();	//print version
char* readline(int fd, char *line);	//read line from file	
void usage();	//print usage
void mygrep(char *pattern, char *file, queue *q);	//normal pattern matching
void mygrep_r(char *pattern, char *dir);	//pattern matching with directory
void mygrep_f(char *pattern, char *file, queue *q);	//pattern matching - obtain pattern from file
char* getArg(int argc, char *argv[]);	//get command line arguments
void getFileFromDir(char *basePath, char *pattern, queue *q);	//get file names in particular directory
void writeFile(char *str);	//write file names in temp.txt
int regexEval(char *pattern, char *str);	//compile and compare regular expression
void regexMatch(char *pattern, char *file, queue *q);	//pattern mathcing with regular expression
void printLine(char *pattern, char *line);	//print line with color effects
void mygrep_stdin(char *pattern);

