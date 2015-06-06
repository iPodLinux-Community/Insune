#include <string.h>
#include <stdio.h>
#include "exception.h"

Exception::Exception(const char *m)
{
	message = strdup(m);
	int nptrs = Hardware::backtrace(buffer, EXCEPTION_BUFSIZ);
	strings = Hardware::backtraceSymbols(buffer, nptrs);
	if (strings == NULL) {
		perror("backtrace_symbols");
		return;
	}

	for (int i = 1; i < nptrs; ++i)
		stackTrace.append(strings[i]);
}

const char *Exception::getMessage() const
{
	return message;
}

void Exception::printStackTrace() const
{
	list<const char *>::iterator it = stackTrace.start();
	for (; it != stackTrace.end(); ++it)
		fprintf(stderr, "%s\n", (*it)); 
}

const list<const char *> &Exception::getStackTrace() const
{
	return stackTrace;
}

Exception::~Exception()
{
	free(message);
	if (strings)
		free(strings);
}
