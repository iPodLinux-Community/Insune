#include <execinfo.h>
#include <dlfcn.h>
#include "hardware.h"

void Hardware::init(int argc, char **argv)
{ }

void Hardware::destroy()
{ }

int Hardware::backtrace(void **buffer, int size)
{
	return ::backtrace(buffer, size);
}

char const **Hardware::backtraceSymbols(void *const *buffer, int size)
{
	return (char const **)backtrace_symbols(buffer, size);
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
