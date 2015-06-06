#include "smartpointer.h"
#include "unitAssert.h"

class TClass {
 public:
	TClass(int *v)
	{
		this->v = v;
		(*v)++;
	}
	~TClass()
	{
		--(*v);
	}
	int *v;
};

const char *smartpointerGetName()
{
	return "SmartPointer";
}

bool smartpointerUnitTest()
{
	int val = 54;
	TClass *cl = new TClass(&val);
	unitAssert(*cl->v == 55);
	{
		SmartPointer<TClass> spi(cl);
		SmartPointer<TClass> ref = spi;
		unitAssert(*spi->v == 55);
		*spi->v = 89;
		unitAssert(*ref->v == 89);
		unitAssert(val == 89);
	}
	unitAssert(val == 88);
	return true;
}
