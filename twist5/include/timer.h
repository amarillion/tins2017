#ifndef TIMER_H
#define TIMER_H

#include "component.h"
#include "widget.h"
#include <memory>

#include "componentbuilder.h"

class ITimer
{
public:
	virtual int getCounter() = 0;
};

class Timer : public IComponent
{
	int counter;
	int msg;
	int maxCounter;
	int mode;
	ActionFunc action;
public:
	Timer(int maxCounter, int msg, int mode = ONCE) :
		counter(0), msg(msg), maxCounter(maxCounter), mode(mode), action()
		{ setAwake(true); setVisible(false); }

	Timer(int maxCounter, ActionFunc action, int mode = ONCE) :
		counter(0), msg(0), maxCounter(maxCounter), mode(mode), action(action)
		{ setAwake(true); setVisible(false); }

	virtual ~Timer();
	virtual void update() override;

	enum { ONCE, REPEATING };

	static ComponentBuilder<Timer> build(int ticks, int msg, int mode = ONCE)
	{
		return ComponentBuilder<Timer>(std::make_shared<Timer>(ticks, msg, mode));
	}

	static ComponentBuilder<Timer> build(int ticks, ActionFunc action, int mode = ONCE)
	{
		return ComponentBuilder<Timer>(std::make_shared<Timer>(ticks, action, mode));
	}

	virtual std::string const className() const override { return "Timer"; }

};

typedef std::shared_ptr<Timer> TimerPtr;

#endif
