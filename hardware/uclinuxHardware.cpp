#include <dlfcn.h>
#include <ucdl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "exception.h"
#include "hardware.h"
#include "insune.h"

void Hardware::init(int argc, char **argv)
{
	char exepath[256];
	if (argv[0][0] == '/')
		strcpy(exepath, argv[0]);
	else
		sprintf(exepath, "/bin/%s", argv[0]);
	if ((uCdl_init(exepath)) == 0) {
		fprintf(stderr, "uCdl init failed: %s\n", uCdl_error());
		throw Exception(uCdl_error());
	}

	close(2);
	fopen("/var/log/insune.log", "a");
	warn("==========================================\nLog opened %d\n",
		time(NULL));
}

void Hardware::destroy()
{ }

int Hardware::backtrace(void **buffer, int size)
{
#if defined(TARGET_CPU_ARM) || defined(IPOD)
	unsigned long PC, *FP, retaddr;
	asm("mov %0, r11" : "=r" (FP) : );
	PC = *(unsigned long  *)((char *)FP + 0x4c);
	FP = *(unsigned long **)((char *)FP + 0x3c);
	int i;
	for (i = 0; i < 10; ++i) {
		retaddr = *(FP - 1);
		buffer[i] = (void*)retaddr;
		FP = (unsigned long *)*(FP - 3);
	}
	return i;
#else
#warning "Unknown target for backtrace"
	return 0;
#endif
}

char const **Hardware::backtraceSymbols(void *const *buffer, int size)
{
	char const **strings = (const char**)xmalloc(size * sizeof(const char*));

	for (int i = 0; i < size; ++i)
		strings[i] = resolveSymbolName(buffer[i]);
	return strings;
}

handle_t Hardware::openLibrary(const char *path)
{
	return dlopen(path, RTLD_LAZY);
}

void Hardware::closeLibrary(handle_t handle)
{
	dlclose(handle);
}

void *Hardware::resolveSymbol(handle_t handle, const char *symbol)
{
	return dlsym(handle, symbol);
}

const char *Hardware::resolveSymbolName(const void *symbol)
{
	return uCdl_resolve_addr((unsigned long)symbol, NULL, NULL);
}

const char *Hardware::getSymbolError()
{
	return dlerror();
}
