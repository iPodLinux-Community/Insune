#include "insune.h"
#include "transition.h"

unsigned int getMS(void);

Transition::Transition(Canvas *f, TransitionDirection dir)
 : canvas(f), direction(dir)
{ }

Transition::~Transition()
{ }

Canvas *Transition::getCanvas()
{
	return canvas;
}

FadeTransition::FadeTransition(Canvas *f, TransitionDirection dir)
 : Transition(f, dir), currentStep(0), done(false)
{
	getCanvas()->object()->opacity = 0;
	lastTime = 0;
}

FadeTransition::~FadeTransition()
{
}

bool FadeTransition::isFinished() const
{
	return done;
}

void FadeTransition::step()
{
	if (lastTime > getMS() - 50)
		return;
	if (++currentStep >= 8) {
		done = true;
		getCanvas()->object()->opacity = 0xff;
		getCanvas()->object()->dirty = 1;
		return;
	}
	
	if (direction == TRANSITION_IN)
		getCanvas()->object()->opacity = 31 * currentStep;
	else
		getCanvas()->object()->opacity = 31 * (7 - currentStep);
	getCanvas()->object()->dirty = 1;
	lastTime = getMS();
}
