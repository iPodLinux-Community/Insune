#include "list.h"
#include "unitAssert.h"

const char *listGetName()
{
	return "List";
}

bool listUnitTest()
{
	list<int> intlist;
	for (int i = 0; i < 100; ++i)
		intlist.append(i);

	unitAssert(intlist.count() == 100);
	unitAssert(intlist.pop() == 0);
	unitAssert(intlist.last() == 99);
	unitAssert(intlist.first() == 1);
	intlist.push(656);
	unitAssert(*intlist.start() == 656);
	intlist.sort();
	unitAssert(intlist.nth(1) == 2);
	unitAssert(intlist.last() == 656);
	intlist.destroy();
	unitAssert(intlist.count() == 0);
	return true;
}
