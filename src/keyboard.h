#include <stdbool.h>

struct key_thread_args {
	char* logfilename;
	char* statfilename;
	bool exit;
};

char* KeyCodeToStr(int code);

void out(char* str, long time, int i);

int outputstats();

char* itoletter(int i);

int lettertoi(char* i);

void initLogs();

int StrTimer(char* str, char* mode);

void * key_thread(void * args);
