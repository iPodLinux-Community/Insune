#include "config.h"
#include <dlfcn.h>
#include <stdlib.h>
#if MACOS_VERSION >= 0x1050
#include <execinfo.h>
#endif
#include "hardware.h"
#include "insune.h"

void Hardware::init(int argc, char **argv)
{ }

void Hardware::destroy()
{ }

int Hardware::backtrace(void **buffer, int size)
{
#if MACOS_VERSION < 0x1050
	#warning "OSX version < 10.5 doesn't support backtrace"
	return 0;
#else
	return backtrace(buffer, size);
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
	Dl_info info;
	if (dladdr(symbol, &info) == 0)
		return 0;
	return info.dli_sname;
}

const char *Hardware::getSymbolError()
{
	return dlerror();
}
