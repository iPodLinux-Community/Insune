#ifndef _TERMINAL_H_
#define _TERMINAL_H_
#include <termios.h> 
#include <sys/time.h>


class Terminal
{
 public:
	Terminal();
	~Terminal();
 private:
	struct termios settings;
};

#endif
