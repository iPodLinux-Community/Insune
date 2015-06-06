#include "unitAssert.h"

class Basic
{
 public:
	Basic(int *a, int &b)
	: b(b)
	{
		this->a = a;
		(*a)++;
		b++;
	}

	void func() throw (int)
	{
		throw 5;
	}
	~Basic()
	{
		(*a)--;
		b--;
	}
	int *a;
	int &b;
};

const char *basicGetName()
{
	return "Basic C++";
}

bool basicUnitTest()
{
	int a = 66, b = 55;
	{
		Basic ba(&a, b);
		unitAssert(a == 67);
		unitAssert(b == 56);
		try {
			ba.func();
			printf("exception Fail!\n");
			return false;
		} catch (int) {
		}
		Basic *bb = new Basic(&a, b);
		unitAssert(a == 68);
		unitAssert(b == 57);
		delete bb;
		unitAssert(a == 67);
		unitAssert(b == 56);
	}
	unitAssert(a == 66);
	unitAssert(b == 55);
	return true;
}
