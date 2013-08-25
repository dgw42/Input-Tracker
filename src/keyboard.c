#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include "keyboard.h"
#define BIT(c, x)       ( c[x/8]&(1<<(x%8)) )
#define TRUE            1
#define FALSE           0
 
#define KEYSYM_STRLEN   64
 
/* Global variables */
static Display *disp;
static FILE* logfile;
static FILE* statfile;
 
typedef struct log {
	char* str; //the letter this log is assigned to
	unsigned int numpressed; //total number of keypresses
	struct timeval tvs;
	struct timeval tve;
	long time; // stores time pressed
	long longesttime; // stores longest time pressed
	double avgtime; // avg time key has been pressed
} log;

log** logs;
 
void cleaner(int sig)
{
   outputStats(statfile); 
	fprintf(stderr, "\nGot Ctrl+C closing...\n");
	fflush(stdout);
	exit(0);
}

char* itoletter(int i)
{
	if (i == 0) return "a";
	else if (i == 1) return "b";
	else if (i == 2) return "c";
	else if (i == 3) return "d";
	else if (i == 4) return "e";
	else if (i == 5) return "f";
	else if (i == 6) return "g";
	else if (i == 7) return "h";
	else if (i == 8) return "i";
	else if (i == 9) return "j";
	else if (i == 10) return "k";
	else if (i == 11) return "l";
	else if (i == 12) return "m";
	else if (i == 13) return "n";
	else if (i == 14) return "o";
	else if (i == 15) return "p";
	else if (i == 16) return "q";
	else if (i == 17) return "r";
	else if (i == 18) return "s";
	else if (i == 19) return "t";
	else if (i == 20) return "u";
	else if (i == 21) return "v";
	else if (i == 22) return "w";
	else if (i == 23) return "x";
	else if (i == 24) return "y";
	else if (i == 25) return "z";
}

int lettertoi(char* l)
{
	if (strcmp(l, "a") == 0) return 0;
	else if (strcmp(l, "b") == 0) return 1;
	else if (strcmp(l, "c") == 0) return 2;
	else if (strcmp(l, "d") == 0) return 3;
	else if (strcmp(l, "e") == 0) return 4;
	else if (strcmp(l, "f") == 0) return 5;
	else if (strcmp(l, "g") == 0) return 6;
	else if (strcmp(l, "h") == 0) return 7;
	else if (strcmp(l, "i") == 0) return 8;
	else if (strcmp(l, "j") == 0) return 9;
	else if (strcmp(l, "k") == 0) return 10;
	else if (strcmp(l, "l") == 0) return 11;
	else if (strcmp(l, "m") == 0) return 12;
	else if (strcmp(l, "n") == 0) return 13;
	else if (strcmp(l, "o") == 0) return 14;
	else if (strcmp(l, "p") == 0) return 15;
	else if (strcmp(l, "q") == 0) return 16;
	else if (strcmp(l, "r") == 0) return 17;
	else if (strcmp(l, "s") == 0) return 18;
	else if (strcmp(l, "t") == 0) return 19;
	else if (strcmp(l, "u") == 0) return 20;
	else if (strcmp(l, "v") == 0) return 21;
	else if (strcmp(l, "w") == 0) return 22;
	else if (strcmp(l, "x") == 0) return 23;
	else if (strcmp(l, "y") == 0) return 24;
	else if (strcmp(l, "z") == 0) return 25;
}

void initLogs()
{
	logs = malloc(sizeof(log*) * 26);
	int i;
	for(i = 0; i < 26; i++) {
		logs[i] = malloc(sizeof(log));
		logs[i]->str = malloc(2);
		strcpy(logs[i]->str, itoletter(i));
		logs[i]->numpressed = 0;
		logs[i]->time = 0;
		logs[i]->avgtime = 0;
	}
}

int StrTimer(char* str, char* mode)
{
	int i = lettertoi(str);

	if (strcmp(mode,"start") == 0) {
		(logs[i]->numpressed)++;
		gettimeofday(&(logs[i]->tvs), NULL);
	}
	else if(strcmp(mode, "end") == 0) {
		gettimeofday(&(logs[i]->tve), NULL);	
		
		logs[i]->time = (logs[i]->tve.tv_sec - logs[i]->tvs.tv_sec) * 1000 +
			(logs[i]->tve.tv_usec - logs[i]->tvs.tv_usec) / 1000;

		logs[i]->avgtime = ((logs[i]->avgtime * (logs[i]->numpressed-1)) +
				logs[i]->time) / logs[i]->numpressed;

		if (logs[i]->time > logs[i]->longesttime)
			logs[i]->longesttime = logs[i]->time;
	}

	return 0;
}

void * key_thread(void * args)
{
	struct key_thread_args* key_args = (struct key_thread_args*)args;
	char	*char_ptr,
			buf1[32],
			buf2[32],
			*keys,
			*saved;
	int 	i,
	  	 	delay = 10000;

	char* logfilename = key_args->logfilename;
	char* statfilename = key_args->statfilename;
	logfile = fopen(logfilename, "w");
	statfile = fopen(statfilename, "w");

	signal(SIGINT, cleaner);
	signal(SIGTERM, cleaner);
 
  	initLogs();
 
  	/* setup Xwindows */
	disp = XOpenDisplay(NULL);
	if (disp == NULL) {
		fprintf(stderr, "Cannot open display\n");
		exit(1);
	}
	XSynchronize(disp, TRUE);
 
	/* setup buffers */

	saved = buf1;
	keys = buf2;
    
	XQueryKeymap(disp, saved);
	while (1) {
		/* find changed keys */
		XQueryKeymap(disp, keys);
		for (i = 0; i < 32*8; i++) {
			/* if the key data is different that the default we saved before */
		  	int kbit = BIT(keys, i);
			int sbit = BIT(saved, i);
			if(kbit > 0) kbit = 1;
			if(sbit > 0) sbit = 1;
			if (BIT(keys, i) != BIT(saved, i)) {
				register char* str;
				str = (char*) KeyCodeToStr(i);
				int j = lettertoi(str);
				if(j >= 0 && j < 26) {
					if (!sbit && kbit) {
						StrTimer(str, "start");
					}
					if (sbit && !kbit) {
						StrTimer(str, "end");
						out(str, logs[j]->time, logs[j]->numpressed);
					}
				}
				fflush(stdout); /* in case user is writing to a pipe */
			}
		}

		char_ptr = saved;
		saved = keys;
		keys = char_ptr;

		if(key_args->exit) {
			outputStats();
			fclose(statfile);
			fclose(logfile);
			return;
		}
	}

}
 
void out(char* str, long time, int i) 
{
	fprintf(logfile, "%s for %ld ms %d times\n", str, time, i);
}

/* This part takes the keycode and makes an output string. */
 
char* KeyCodeToStr(int code)
{
    static char *str, buf[KEYSYM_STRLEN + 1];
    int index = 0;
    KeySym  keysym;

    keysym = XKeycodeToKeysym(disp, code, index);
    if (NoSymbol == keysym) 
		 return "";
 
    /* convert keysym to a string, copy it to a local area */
    str=XKeysymToString(keysym);
 
    if (str == NULL) 
		 return "";
    strncpy(buf, str, KEYSYM_STRLEN); 
	 buf[KEYSYM_STRLEN] = 0;
 
	return buf;
}

/** creates the statistics file given the FILE* sf */
int outputStats()
{
	fprintf(statfile, "\n\n\t\t\t\t\t\tKEYLOG STATISTICSFILE\n===========================================================================\n\n");
	fprintf(statfile, "TOTAL KEYLOG STATISTICS\n--------------------------------------------------------------\n");
	int i;
	long avgTimeTotal = 0;
	int keypressesTotal = 0;
	int keytypes = 0;
	for (i = 0; i < 26; i++) {
		if (logs[i]->avgtime != 0)
			keytypes++;
		avgTimeTotal += logs[i]->avgtime;
		keypressesTotal += logs[i]->numpressed;
	}
	avgTimeTotal = avgTimeTotal / keytypes;
	fprintf(statfile, "\tAverage Keypress Time:\t\t\t\t%ld milliseconds\n", avgTimeTotal);
	fprintf(statfile, "\tTotal Number of Keypresses:\t\t%d presses\n",
			keypressesTotal);
	fprintf(statfile, "---------------------------------------------------------------------------\n\n");
	fprintf(statfile, "KEYLOG STATISTICS BY LETTER\n----------------------------------------------------------------------------\n\n");
	fprintf(statfile, "LETTER\t\t||\tNumber Times Pressed\t||\tAverage Time\t||\tLongest Time Pressed\t||\n--------------------------------------------------------------------------\n");
	for(i = 0; i < 26; i++) {
		fprintf(statfile, "\t%s\t\t\t\t\t\t%d\t\t\t\t\t\t%f\t\t\t\t\t\t%ld\n", logs[i]->str, logs[i]->numpressed, logs[i]->avgtime, logs[i]->longesttime);
	}
	fprintf(statfile, "-------------------------------------------------------------------------------------\n\n");
	fprintf(statfile, "=====================================================================================");
}
