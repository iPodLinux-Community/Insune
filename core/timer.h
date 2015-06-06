#ifndef _TIMER_T_
#define _TIMER_T_

#include "action.h"

class Timer
{
 public:
	Timer(unsigned int ms, ActionBase *action);

	void setSingleUse(bool o = true);
	bool isTime() const;
	void performAction();

	bool isSingleUse() const;
	ActionBase *getAction() const;

 private:
	unsigned int last;
	unsigned int next;
	ActionBase *action;
	bool singleUse;
};

#endif
