#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include "list.h"
#include "hardware.h"

#define EXCEPTION_BUFSIZ 100

class Exception
{
 public:
	Exception(const char *message);
	virtual ~Exception();
	const char *getMessage() const;
	void printStackTrace() const;
	const list<const char *> &getStackTrace() const;

 private:
	char *message;
	char const ** strings;
	list<const char *> stackTrace;
	void *buffer[EXCEPTION_BUFSIZ];
};

#endif
