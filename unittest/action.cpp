#include "action.h"

class Foo
{
 public:
	Foo()
	{
		success = false;
		action = new Action<Foo, int>(this, &Foo::func);
		((Action<Foo, int> *)action)->setData(123);
		action->callback();
	}
	~Foo()
	{
		delete (Action<Foo, int> *)action;
	}

	void func(int v)
	{ success = (v == 123); }

	ActionBase *action;
	bool success;
};

const char *actionGetName()
{
	return "Action";
}

bool actionUnitTest()
{
	Foo f = Foo();
	return f.success;
}
