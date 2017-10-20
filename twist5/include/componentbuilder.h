/*
 * componentBuilder.h
 *
 *  Created on: 3 Aug 2012
 *      Author: martijn
 */

#ifndef COMPONENTBUILDER_H_
#define COMPONENTBUILDER_H_

#include <memory>
#include "motion.h"

struct ALLEGRO_FONT;

template <typename W>
class ComponentBuilder {
public:
	ComponentBuilder(std::shared_ptr<W> aComponent) : component(aComponent) {}
	std::shared_ptr<W> get() { return component; }

	ComponentBuilder<W> &motion(IMotionPtr motion);
	ComponentBuilder<W> &linear(int x, int y);

	ComponentBuilder<W> & center(int prefw = 200, int prefh = 40);
	ComponentBuilder<W> & xy(double x, double y);
	ComponentBuilder<W> & xywh(double x, double y, double w, double h);
	ComponentBuilder<W> & font(ALLEGRO_FONT *font);
	ComponentBuilder<W> & layout(int flags, int x1, int y1, int x2, int y2);
private:
	std::shared_ptr<W> component;
};

#include "componentbuilderimpl.h"

#endif /* COMPONENTBUILDER_H_ */
