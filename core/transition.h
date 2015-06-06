#ifndef _TRANSITION_H_
#define _TRANSITION_H_
#include "canvas.h"

enum TransitionDirection {
	TRANSITION_IN,
	TRANSITION_OUT
};

class Transition
{
 public:
	Transition(Canvas *to, TransitionDirection dir);
	virtual ~Transition();

	virtual void step() = 0;
	virtual bool isFinished() const = 0;
	Canvas *getCanvas();

 protected:
	Canvas *canvas;
	TransitionDirection direction;
};

class FadeTransition : public Transition
{
 public:
	FadeTransition(Canvas *to, TransitionDirection dir);
	~FadeTransition();

	bool isFinished() const;
	void step();
 private:
	int currentStep;
	bool done;
	unsigned int lastTime;
};

#endif
