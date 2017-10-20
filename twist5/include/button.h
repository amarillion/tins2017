#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "componentbuilder.h"
#include "widget.h"

class Button : public TextWidget
{
private:
    int action;
    ActionFunc actionFunc;

	void doAction();
protected:
	virtual void doDraw (const GraphicsContext &gc) override;
    bool fFocus;
    bool pressed;
    bool enabled;

    Bitmap icon;
    void parseAndSetText(std::string value);
public:
    int getAction() { return action; }
	virtual void handleMessage(std::shared_ptr<IComponent> src, int msg) override;
	Button () : Button (MSG_ACTIVATE, "") {}
	Button (int action, std::string text, ALLEGRO_BITMAP* aIcon = NULL) : action(action), actionFunc(), fFocus(false), pressed(false), enabled(true)
	{
		parseAndSetText(text);
		if (aIcon != NULL) icon = Bitmap(aIcon); else icon = Bitmap();
		setBorder(Skin::BUTTON);
		SetFlag(D_DOUBLEBUFFER);
	}

	virtual ~Button() {}
    virtual bool wantsFocus () override { return true; }
    virtual void handleEvent(ALLEGRO_EVENT &event) override;
    virtual double getPreferredWidth() override;

    void setEnabled (bool value) { enabled = value; dirty = true; }
    bool isEnabled() { return enabled; }

    bool hasFocus() { return fFocus; }
    void setFocus(bool val) { fFocus = val; dirty = true; }
	bool HasFocus() { return fFocus; } // TODO deprecated

    void setIcon(int i) { icon = GetSkin()->GetBitmap(i); dirty = true; }

    void setActionCode(int value) { action = value; }
    void setActionFunc(ActionFunc value) { actionFunc = value; }

    static ComponentBuilder<Button> build(int action, std::string text, int accelerator_key = -1, ALLEGRO_BITMAP* icon = NULL);
    static ComponentBuilder<Button> build(ActionFunc actionFunc, std::string text, int accelerator_key = -1, ALLEGRO_BITMAP* icon = NULL);

    // MASking leftovers...
	void MakeExit(); // make this button exit the dialog

    void setToggle(bool value) { if (value) SetFlag(D_TOGGLE); else ClearFlag(D_TOGGLE); }
	void Toggle() { pressed = !pressed; dirty = true; }
	void Select() { pressed = true; dirty = true; } //TODO: merge select and deselect into a single "setSelected"
	void Deselect() { pressed = false; dirty = true; }
	bool Selected() { return pressed; } //TODO: rename to isSelected
	bool isToggleButton() { return TestFlag(D_TOGGLE); }

	virtual void UpdateSize() override
	{
		TextWidget::UpdateSize();
		if (bufferSizeMismatch())
		{
			resetBuffer();
		}
	}

	virtual std::string const className() const override { return "Button"; }
};

#endif
