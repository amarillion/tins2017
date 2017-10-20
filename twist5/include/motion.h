#ifndef MOTION_H
#define MOTION_H

#include <memory>

class IMotion
{
public:
	virtual ~IMotion() { }
	virtual int getdx(int counter) = 0;
	virtual int getdy(int counter) = 0;
};

typedef std::shared_ptr<IMotion> IMotionPtr;

#endif
