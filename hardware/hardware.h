#ifndef _HARDWARE_H_
#define _HARDWARE_H_

typedef void * handle_t;

namespace Hardware {
	void init(int argc, char **argv);
	void destroy();

	/* backtrace functions */
	int backtrace(void **buffer, int size);
	char const **backtraceSymbols(void *const *buffer, int size);

	/* dl functions */
	handle_t openLibrary(const char *path);
	void closeLibrary(handle_t handle);
	void *resolveSymbol(handle_t handle, const char *symbol);
	const char *resolveSymbolName(const void *symbol);
	const char *getSymbolError();
}

#endif
#ifndef _HARDWARE_H_
#define _HARDWARE_H_

class HardwareInterface
{
	void init(int argc, char **argv);
	void reboot();
	void powerOff();
	void diskmode();
};

#endif
