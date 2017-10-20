#include "text.h"
#include <string>
#include <sstream>
#include "color.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "mainloop.h"
#include "textstyle.h"

using namespace std;
using namespace std::placeholders;

/** wrapper for a stretch of text. Each stretch can be styled separately
 * with different colors and fonts. */
class TextSegment
{
private:
	std::string elem;
public:
	explicit TextSegment(std::string val) : elem(val) {}
	std::string &getText() { return elem; }
};

class TextImpl : public Text
{
private:
	ALLEGRO_COLOR color;

	int align;

	bool isPerLetter;
	int letterDelta;
	int style = STYLE_NONE;
	std::shared_ptr<IColorGenerator> letterColorGenerator;
	std::shared_ptr<IMotion> motion;
	std::shared_ptr<IMotion> letterMotion;

	std::shared_ptr<TextModel> model;

	unsigned int typewriterPos;
	int typewritingMode;
	int typewriterDelay;

	std::vector<TextSegment> elems;

	size_t showlines = 0;
	int lineh;
	bool blink;

	void split (const std::string &s);

	void refresh() {
		showlines = elems.size();
		blink = false;
	}

protected:
	virtual void changed(int code = 0) override
	{
		elems.clear();
		split(model->getText());
		refresh();
	}

public:

	enum { OFF, LINE_BY_LINE, LETTER_BY_LETTER };

	TextImpl(ALLEGRO_COLOR color, int _align, string const &aText) :
		color(color), align(_align), isPerLetter(false),
		letterDelta(100), letterColorGenerator(), motion(), letterMotion(), model(),
		typewriterPos(0), typewritingMode(OFF), typewriterDelay(0),
		elems(), lineh(0), blink(false)
	{
		model = make_shared<TextModel>(aText); // generate default model
		model->AddListener(this);
		changed(1); // update from model
		setDimension(300, 20); //TODO: better default w, h...
	}

	virtual void draw(const GraphicsContext &gc) override;
	virtual void update() override;

	void startTypewriter(int mode, int delay = 0)
	{
		typewritingMode = mode;
		switch (mode)
		{
		case LETTER_BY_LETTER:
			typewriterPos = 0;
			typewriterDelay = (delay <= 0) ? 1 : delay;
			break;
		case LINE_BY_LINE:
			showlines = 1;
			typewriterDelay = (delay <= 0) ? 48 : delay;
			break;
		default:
			break;
		}
	}

	virtual void setAlignment(int value) override
	{ align = value; }
	virtual void setColor (ALLEGRO_COLOR _color) override
	{ color = _color; }

	virtual void setLetterColorGenerator (std::shared_ptr<IColorGenerator> generator) override
	{ letterColorGenerator = generator; isPerLetter = true; }

	virtual void setText (std::string const &value) override
	{ model->setText(value); }

	virtual void setTextModel(std::shared_ptr<TextModel> const &value) override
	{ if (model != value) { model = value; model->AddListener(this); } }

	virtual void setMotion (std::shared_ptr<IMotion> const &_motion) override
	{ motion = _motion; }

	virtual void setLetterMotion (std::shared_ptr<IMotion> const &_letterMotion) override
	{ letterMotion = _letterMotion; isPerLetter = true; }

	virtual void setStyle (int _style) override
	{ style = _style; }

	virtual void setLetterDelta (int delta) override { letterDelta = delta; }

	virtual void setTextf(const char *msg, ...) override
	{
		char buf[256];

		va_list ap;
		va_start(ap, msg);
		vsnprintf (buf, sizeof(buf), msg, ap);
		va_end(ap);

		model->setText(buf);
	}

};

// TODO maybe - move this to a separate class for processing and formatting text.
void TextImpl::split(const string &s)
{
	stringstream ss(s);
	string item;
	while(std::getline(ss, item, '\n')) {
		elems.push_back(TextSegment(item));
	}
}

void TextImpl::draw(const GraphicsContext &gc)
{
	if (!visible) return; // do not draw the invisible...
	if (!sfont) return; //TODO: warn("Attempt to draw text without a valid font")

	int ybase = gety() + gc.yofst;
	int xbase = getx() + gc.xofst;

	if (motion)
	{
		xbase += motion->getdx(counter);
		ybase += motion->getdy(counter);
	}

	int lineh = al_get_font_line_height(sfont);
	int lineRemain = (typewritingMode == LINE_BY_LINE) ? showlines : elems.size();
	size_t typewriterRemain = typewriterPos;

	function<void(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, char const *text)> textFunc;

	switch (style)
	{
	case STYLE_DROP_SHADOW: textFunc = bind (draw_shaded_text, _1, _2, BLACK, _3, _4, _5, _6); break;
	case STYLE_CLEAR_BACKGROUND: textFunc = bind (draw_text_with_background, _1, _2, BLACK, _3, _4, _5, _6); break;
	case STYLE_NONE: default: textFunc = al_draw_text; break;
	}

	int yco = ybase;
	string partial;
	for (vector<TextSegment>::iterator i = elems.begin(); i != elems.end() && lineRemain > 0; ++i, lineRemain--)
	{
		string &strVal = (*i).getText();
		bool typewriterActive = (typewritingMode == LETTER_BY_LETTER) && strVal.size() >= typewriterRemain;
		string activeText = typewriterActive ? strVal.substr(0, typewriterRemain) : strVal;

		if (isPerLetter)
		{
			int xco;
			switch (align)
			{
			case ALLEGRO_ALIGN_RIGHT:
				xco = xbase + getw() - al_get_text_width(sfont, activeText.c_str()); break;
			case ALLEGRO_ALIGN_CENTER:
				xco = xbase + (getw() - al_get_text_width(sfont, activeText.c_str())) / 2; break;
			default:
				xco = xbase; break;
			}

			int pos;
			char textbuf[2] = " ";
			int tlen = activeText.size();
			int ddx = 0;
			int ddy = 0;

			for (int pos = 0; pos < tlen; ++pos)
			{
				ALLEGRO_COLOR lettercolor =
					(letterColorGenerator == NULL) ?
					color :
					letterColorGenerator->getColor(MSEC_FROM_TICKS(counter), pos);
				textbuf[0] = activeText[pos];

				if (letterMotion)
				{
					ddx = letterMotion->getdx(counter + letterDelta * pos);
					ddy = letterMotion->getdy(counter + letterDelta * pos);
				}

				textFunc(sfont, lettercolor, xco + ddx, yco + ddy, ALLEGRO_ALIGN_LEFT, textbuf);

				xco += al_get_text_width (sfont, textbuf);
			}
		}
		else
		{
			int xco;
			switch (align)
			{
			case ALLEGRO_ALIGN_RIGHT:
				xco = xbase + getw(); break;
			case ALLEGRO_ALIGN_CENTER:
				xco = xbase + getw() / 2; break;
			default:
				xco = xbase; break;
			}

			textFunc(sfont, color, xco, yco, align, activeText.c_str());
		}

		typewriterRemain -= activeText.size();
		yco += lineh;
	}

}

void TextImpl::update() {
	IComponent::update();

	switch (typewritingMode)
	{
	case Text::LINE_BY_LINE:
	{
		if (counter % typewriterDelay == 0)
		{
			showlines++;
			if (showlines > elems.size())
			{
				pushMsg(MSG_ANIMATION_DONE);
				typewritingMode = Text::OFF;
			}
		}
		break;
	}
	case Text::LETTER_BY_LETTER:
	{
		if (counter % typewriterDelay == 0)
		{
			typewriterPos++;

			// send message when animation is finished
			if (typewriterPos >= model->getText().size())
			{
				pushMsg(MSG_ANIMATION_DONE);
				typewritingMode = Text::OFF;
				typewriterPos = 0;
			}
		}
		break;
	}
	default:
		// do nothing;
		break;
	}
}

ComponentBuilder<Text> Text::build(int _align, std::string val)
{
	//TODO: look up colour from skin.
	return build(BLACK, _align, val);
}

ComponentBuilder<Text> Text::build(ALLEGRO_COLOR _color, std::string val)
{
	return build(_color, ALLEGRO_ALIGN_CENTER, val);
}

ComponentBuilder<Text> Text::build(ALLEGRO_COLOR _color, int _align, std::string val)
{
	auto result = make_shared<TextImpl>(_color, _align, val);
	return ComponentBuilder<Text>(result);
}

ComponentBuilder<Text> Text::buildf(ALLEGRO_COLOR _color, int _align, const char *msg, ...)
{
	char buf[256];

	va_list ap;
	va_start(ap, msg);
	vsnprintf (buf, sizeof(buf), msg, ap);
	va_end(ap);

	auto result = make_shared<TextImpl>(_color, _align, buf);
	return ComponentBuilder<Text>(result);
}

ComponentBuilder<Text> Text::buildf(ALLEGRO_COLOR _color, const char *msg, ...)
{
	char buf[256];

	va_list ap;
	va_start(ap, msg);
	vsnprintf (buf, sizeof(buf), msg, ap);
	va_end(ap);

	auto result = make_shared<TextImpl>(_color, ALLEGRO_ALIGN_LEFT, buf);
	return ComponentBuilder<Text>(result);
}
