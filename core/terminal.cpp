#include <string.h>
#include "terminal.h"

Terminal::Terminal()
{
	struct termios nsettings;
	tcgetattr(0, &settings);
	memcpy(&nsettings, &settings, sizeof(struct termios));
	nsettings.c_lflag &= ~(ICANON | ECHO | ISIG);
	nsettings.c_iflag &= ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON);
	nsettings.c_cc[VTIME] = 0;
	tcgetattr(0, &settings);
	nsettings.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &nsettings);
}

Terminal::~Terminal()
{
	tcsetattr(0, TCSANOW, &settings);
}
