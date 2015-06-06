#include <stdio.h>
#include <stdlib.h>
#include "unittest.h"
#ifdef __linux__
#include <sys/ioctl.h>
#endif

bool unitTest()
{
	int cols = 34;
#ifdef __linux__
	struct winsize {
		unsigned short  ws_row;
		unsigned short  ws_col;
		unsigned short  ws_xpixel;
		unsigned short  ws_ypixel;
	} size;
	ioctl(0, TIOCGWINSZ, (char *)&size);
	cols = size.ws_col - 6;
#endif
	bool ret = true;
	printf("\033[1mStarting unit tests...\033[0m\n");
	for (int i = 0; tests[i].name != NULL; ++i) {
		printf("%s...   ", (* tests[i].name)());
		fflush(stdout);
		if (!(* tests[i].test)()) {
			printf("\033[%dG\033[0;31mFailed\033[0m\n", cols);
			ret = false;
		}
		else
			printf("\033[%dG\033[0;32mPassed\033[0m\n", cols);
	}
	return ret;
}
