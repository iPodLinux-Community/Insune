#include <sys/time.h>
#include "timer.h"

unsigned int getMS(void)
{
        struct timeval tv;
        gettimeofday(&tv, 0);
        return (unsigned int)((tv.tv_sec % 0x3fffff)*1000 + tv.tv_usec/1000);
}

Timer::Timer(unsigned int ms, ActionBase *action)
{
	last = getMS();
	next = last + ms; // overflow allowed
	this->action = action;
	singleUse = false;
}

void Timer::setSingleUse(bool set)
{
	singleUse = set;
}

bool Timer::isTime() const
{
	unsigned int now = getMS();
	return (last < now && next <= now) || // normal
	       (last > now && (next <= now || next > last)); // overflow
}

bool Timer::isSingleUse() const
{
	return singleUse;
}

void Timer::performAction()
{
	action->callback();
	if (!singleUse) {
		if (next < last) { // fix overflow
			next -= last;
			last = 0;
		}
		unsigned int now = next;
		next = now + (next - last);
		last = now;
	}
}

ActionBase *Timer::getAction() const
{
	return action;
}
