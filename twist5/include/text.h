#ifndef TEXT_H
#define TEXT_H

#include "component.h"
#include <vector>
#include <allegro5/allegro_font.h>
#include "componentbuilder.h"
#include "data.h"

class IColorGenerator
{
public:
	virtual ALLEGRO_COLOR getColor(int msec, int index) = 0;
	virtual ~IColorGenerator() {}
};

class TextModel : public DataWrapper
{
private:
	std::string text;
public:
	TextModel() : text() {}
	TextModel(std::string const &initialText) : text(initialText) {}
	std::string &getText() { return text; }
	virtual void setText(std::string const &value)
	{
		if (value != text)
		{
			text = value;
			FireEvent(1);
		}
	}
	virtual ~TextModel() {}
};

class Text : public IComponent, public DataListener
{
protected:
	virtual void changed(int code = 0) override = 0;
public:
	enum { OFF, LINE_BY_LINE, LETTER_BY_LETTER };
	enum { STYLE_NONE = 0, STYLE_DROP_SHADOW = 1, STYLE_CLEAR_BACKGROUND };

	virtual void draw(const GraphicsContext &gc) override = 0;
	virtual void update() override = 0;

	virtual void startTypewriter(int mode, int delay = 0) = 0;
	virtual void setAlignment(int value) = 0;
	virtual void setColor (ALLEGRO_COLOR _color) = 0;
	virtual void setLetterColorGenerator (std::shared_ptr<IColorGenerator> generator) = 0;
	virtual void setText (std::string const &value) = 0;
	virtual void setTextModel(std::shared_ptr<TextModel> const &value) = 0;
	virtual void setMotion (std::shared_ptr<IMotion> const &_motion) = 0;
	virtual void setLetterMotion (std::shared_ptr<IMotion> const &_letterMotion) = 0;
	virtual void setLetterDelta (int delta) = 0;
	virtual void setStyle(int style) = 0;

	virtual void onAnimationComplete(ActionFunc actionFunc) = 0;
	static ComponentBuilder<Text> build(int _align, std::string val);
	static ComponentBuilder<Text> build(ALLEGRO_COLOR _color, std::string val);
	static ComponentBuilder<Text> build(ALLEGRO_COLOR _color, int _align, std::string val);
	static ComponentBuilder<Text> buildf(ALLEGRO_COLOR _color, int _align, const char *msg, ...);
	static ComponentBuilder<Text> buildf(ALLEGRO_COLOR _color, const char *msg, ...);

	virtual void setTextf(const char *msg, ...) = 0;
};

#endif
