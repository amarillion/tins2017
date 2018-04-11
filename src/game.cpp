#include "molbi.h"
#include "dissolve.h"
#include "game.h"
#include "color.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

#include "engine.h"
#include "util.h"
#include "textstyle.h"
#include "panel.h"
#include "button.h"

#include "messagebox.h"
#include <algorithm>
#include "timer.h"
#include "text.h"

#include "mainloop.h"
#include "sprite.h"
#include "cardrenderer.h"
#include "layout_const.h"
#include "level.h"
#include "analyzer.h"

using namespace std;

class CodonTableView : public Component {

	virtual void draw(const GraphicsContext &gc) override {

		int x1 = getx() + gc.xofst;
		int y1 = gety() + gc.yofst;

		al_draw_rectangle(x1 + 1, y1 + 1, getw() - 1, geth() - 1, WHITE, 1.0);

		const int COL_WIDTH = getw() / 4;
		const int OUTER_ROW_HEIGHT = geth() / 4;
		const int INNER_ROW_HEIGHT = 16;

		for (int i = 0; i < NUM_NUCLEOTIDES; ++i) {
			for (int j = 0; j < NUM_NUCLEOTIDES; ++j) {
				for (int k = 0; k < NUM_NUCLEOTIDES; ++k) {

					// cell position
					int xco = x1 + j * COL_WIDTH + getx();
					int yco = y1 + i * OUTER_ROW_HEIGHT + k * INNER_ROW_HEIGHT;
					AA aaIdx = codonTable.getCodon(i, j, k);
					const AminoAcidInfo *aai = &aminoAcidInfo[static_cast<int>(aaIdx)];
					if (aai) {
						draw_shaded_text(sfont, BLACK, GREEN, xco, yco, ALLEGRO_ALIGN_LEFT, aai->threeLetterCode.c_str());
					}

				}
			}
		}
	};

};

class GameImpl;

class NucleotideSprite : public Sprite, public enable_shared_from_this<NucleotideSprite> {
private:
	GameImpl *parent;
	const NucleotideInfo *info;
	char code;
	int pos;
	NT idx;
	DissolveEffect dissolve; // TODO - make static but initialize at the right moment...
	bool isAnimating = false;
	const int MAX_COUNTER = 100;
	int counter = 0;
public:
	NucleotideSprite(GameImpl *parent, double x, double y, char code, int pos) : parent(parent), code(code), pos(pos) {
		this->x = x;
		this->y = y;
		w = NT_WIDTH;
		h = NT_HEIGHT;
		idx = getNucleotideIndex(code);
		info = &nucleotideInfo[(int)idx];
		sprite = info->card;
		Assert(sprite, "No valid sprite");
	}

	void drawBitmap(const GraphicsContext &gc) {
		al_draw_bitmap (sprite, x + gc.xofst, y + gc.yofst, 0);
	}

	virtual void draw(const GraphicsContext &gc) {
		if (isAnimating) {
			dissolve.withPattern( (float)counter/(float)MAX_COUNTER,
				[=]() {	drawBitmap(gc); }
			);
		}
		else {
			drawBitmap(gc);
		}
	};

	void update() {
		if (isAnimating) {
			counter++;
			if (counter > MAX_COUNTER) {
				kill();
				isAnimating = false;
			}
		}
	}

	void startDissolve() {
		isAnimating = true;
		counter = 0;
	}

	void moveToPos(int _pos);

	bool validate(int _pos, char _code) {
		return (pos == _pos) && (code == _code);
	}
};

// TODO: add to TextComponent...
class TextCursorComponent : public Component {

	int counter = 0;
public:
	void update() override {
		counter++;
	}

	void draw(const GraphicsContext &gc) override {
		int w = getw();
		int h = geth();
		double x = getx() + gc.xofst;
		double y = gety() + gc.yofst;

		const int INSET = 4;
		if (counter % 50 < 25) {
			al_draw_filled_rectangle(x, y, x + w, y + h, GREY);
		}
		else {
			al_draw_filled_rectangle(x + INSET, y + INSET, x + w - INSET, y + h - INSET, GREY);
		}
	}
};

//TODO - implement
class DissolveAnimator : public Sprite {
	DissolveAnimator (const shared_ptr<Sprite> &target) {
	}
};


class AminoAcidSprite : public Sprite {
private:
	const AminoAcidInfo *info;
	AA aaIdx;
public:
	AminoAcidSprite(double x, double y, AA aaIdx, bool full) : aaIdx(aaIdx) {

		this->x = x;
		this->y = y;
		w = AA_WIDTH;
		h = AA_HEIGHT;
		Assert ((int)aaIdx >= 0 && (int)aaIdx < NUM_AMINO_ACIDS, "Invalid amino acid index");
		info = &aminoAcidInfo[static_cast<int>(aaIdx)];

		if (full) {
			sprite = info->bmpFull;
		}
		else {
			sprite = info->bmpSimple;
		}
	}

};

class MutationCursor : public Sprite {
	GameImpl *parent;
	int pos;
	int counter = 0;
public:
	int getPos() { return pos; }
	DissolveEffect dissolve;
	MutationCursor(GameImpl *parent) : parent(parent), pos(0) {
		Sprite();
		w = 32;
		h = 32;
		x = 0;
		h = 0;
		setPos(0);
		anim = Engine::getResources()->getAnim("handcursor");
	}

	void setPos(int newpos) {
		pos = newpos;
		counter = 0;
	}

	virtual void update() override {
		counter++;
	}
};

class Ribosome : public Sprite, public enable_shared_from_this<Ribosome> {
private:
	GameImpl *parent;
	int aaPos = -1;
	bool animating = false;
public:
	Ribosome(GameImpl *parent) : parent(parent) {
		sprite = Engine::getResources()->getBitmap("RIBOSOME");
		y = RIBOSOME_Y;
		x = -100;
	}

	virtual void handleAnimationComplete() override {
		if (alive) {
			nextAnimation();
		}
	}

	void nextAnimation();

};

class MutationCardSprite : public Sprite {

	MutationId mutationId;
	const MutationInfo *info;
public:
	int origX;
	int origY;
	MutationCardSprite(MutationId mutationId, int xco, int yco) : mutationId(mutationId) {
		w = MUTCARD_W;
		h = MUTCARD_H;
		x = xco;
		y = yco;
		info = &mutationInfo[static_cast<int>(mutationId)];
		origX = xco;
		origY = yco;
		sprite = info->card;
	}

	MutationId getMutationId() { return mutationId; }
};

enum class Mode { SCRIPT_RUNNING, WAIT_FOR_KEY, MUTATION_SELECT, POS_SELECT };

class TextBalloon : public Component {

	virtual void draw (const GraphicsContext &gc) override {
		const double shadow = 10;

		double x1 = getx() + gc.xofst;
		double y1 = gety() + gc.yofst;
		double x2 = x1 + getw() - shadow;
		double y2 = y1 + geth() - shadow;

		const double rx = 10;
		const double ry = 10;

		al_draw_filled_rounded_rectangle(x1 + shadow, y1 + shadow, x2 + shadow, y2 + shadow, rx, ry, GREY);
		al_draw_filled_rounded_rectangle(x1, y1, x2, y2, rx, ry, WHITE);
		al_draw_rounded_rectangle (x1, y1, x2, y2, rx, ry, BLACK, 2.0);
	};
};

class GameImpl : public Game {
private:
	int currentLevel;
public:
	SpriteGroup world;
private:

	// positions in this vector should match the current DNAmodel...
	vector<shared_ptr<NucleotideSprite>> geneByPosition;

	shared_ptr<SpriteGroup> geneGroup;
	shared_ptr<SpriteGroup> targetPeptideGroup;
	shared_ptr<SpriteGroup> currentPeptideGroup;

	DNAModel currentDNA;
	PeptideModel targetPeptide;
	PeptideModel currentPeptide;
	vector<MutationId> currentMutationCards; // TODO: also as model?

	ContainerPtr mutationGroup; // used to calculate layout of mutation cards, does not actully contain them.
	list<shared_ptr<MutationCardSprite>> cards;
	shared_ptr<MutationCursor> mutationCursor;
	list<shared_ptr<MutationCardSprite>>::iterator selectedCard;

	shared_ptr<Text> drText;

	shared_ptr<Container> popup;
	shared_ptr<AnimComponent> patient;
	shared_ptr<TextCursorComponent> cursor;

	ActionFunc popupAction;

	Script::iterator currentScript;
	Script::iterator currentScriptEnd;

	bool uiEnabled = true;
public:

	void changeCardFocus(int delta) {
		if (cards.size() == 0) return;

		int maxIt = cards.size();
		auto oldSelectedCard = selectedCard;

		if (selectedCard == cards.end()) selectedCard = cards.begin();

		while (selectedCard != cards.end() && maxIt >= 0) {
			if (delta > 0) {
				selectedCard++;
				if (selectedCard == cards.end()) selectedCard = cards.begin();
			}
			else {
				if (selectedCard == cards.begin()) {
					// TODO - ugly. No good way to wrap a bidirectional iterator?
					for (size_t i = 0; i < cards.size() - 1; ++i) { selectedCard++; }
				} else {
					selectedCard--;
				}
			}
			maxIt--;
		}

		if (oldSelectedCard != selectedCard) {
			MainLoop::getMainLoop()->playSample(Engine::getResources()->getSample("sound_movecursor"));
		}
	}

	// no draw method... just controlling the game
	//TODO: make private
	Mode mode = Mode::SCRIPT_RUNNING;

	bool hasSavedLevel() override {
		int lev = get_config_int (MainLoop::getMainLoop()->getConfig(), "peppy", "currentLevel", -1);
		return (lev != -1);
	}

	void loadCurrentLevel() override {
		currentLevel = get_config_int (MainLoop::getMainLoop()->getConfig(), "peppy", "currentLevel", 0);
	}

	void saveCurrentLevel() {
		set_config_int (MainLoop::getMainLoop()->getConfig(), "peppy", "currentLevel", currentLevel);
	}

	bool isUIEnabled() { return uiEnabled; }

	GameImpl() : currentLevel(0), world() {

		targetPeptideGroup = make_shared<SpriteGroup>();
		currentPeptideGroup = make_shared<SpriteGroup>();
		geneGroup = make_shared<SpriteGroup>();

		world.push_back(targetPeptideGroup);
		world.push_back(currentPeptideGroup);
		world.push_back(geneGroup);

		ALLEGRO_FONT *builtin_font = Engine::getResources()->getFont("builtin_font");

		add (
			Text::build(BLACK, ALLEGRO_ALIGN_LEFT, "Goal:").font(builtin_font).xy(10, TARGET_PEPT_Y).get()
		);

		add (
			Text::build(BLACK, ALLEGRO_ALIGN_LEFT, "Current\n  Output:").font(builtin_font).xy(10, CURRENT_PEPT_Y).get()
		);

		add (
			Text::build(BLACK, ALLEGRO_ALIGN_LEFT, "Gene:").font(builtin_font).xy(10, GENE_Y).get()
		);

		currentDNA.AddListener( [=] (int code, int pos) {
			currentPeptide.setValue(currentDNA.translate());

			switch(code) {
				default:
					generateGeneSprites();
					break;
				case ListWrapper::DELETE:
					geneSpritesDelete(pos);
					break;
				case ListWrapper::INSERT:
					geneSpritesInsert(pos);
					break;
				case ListWrapper::SINGLE_CHANGE:
					mutateGene(pos);
					break;
			};

		});

		currentPeptide.AddListener( [=] (int code) {
			if (currentPeptideGroup->isVisible()) {
				generateRibosome();
			}
			checkWinCondition();
		});

		targetPeptide.AddListener( [=] (int code) {
			peptideToSprites(targetPeptide, targetPeptideGroup, SECTION_X, TARGET_PEPT_Y, true);
		});

		mutationGroup = make_shared<Container>();
		mutationGroup->setLayout(Layout::RIGHT_BOTTOM_W_H, 10, 10, (BUTTONW * 2) + 30, (BUTTONH + 10) * 8);
		add(mutationGroup);

		auto button = Button::build([=](){ resetLevel(); }, "Reset (F1)")
			.layout(Layout::RIGHT_BOTTOM_W_H, 10, 10, 120, 24).get();
		add(button);

		auto b3 = Button::build([=](){ pushMsg(Engine::E_BEFORE_QUIT); }, "Quit")
			.layout(Layout::LEFT_BOTTOM_W_H, 10, 10, 120, 24).get();
		add(b3);

		Resources *res = Engine::getResources();
		auto img1 = BitmapComp::build(res->getBitmap("DrRaul01")).xywh(0, 10, 130, 130).get();
		add(img1);

		patient = AnimComponent::build(res->getAnim("Bigbunnybed")).layout(Layout::RIGHT_TOP_W_H, 0, 20, 300, 200).get();
		add(patient);

		auto balloon = make_shared<TextBalloon>();
		balloon->setLayout(Layout::LEFT_TOP_RIGHT_H, 135, 5, 250, 120);
		add (balloon);
		drText = Text::build(BLACK, ALLEGRO_ALIGN_LEFT, "").layout (Layout::LEFT_TOP_RIGHT_H, 140, 15, 255, 110).get();
		add(drText);

		cursor = make_shared<TextCursorComponent>();
		cursor->setLayout (Layout::RIGHT_TOP_W_H, 270, 84, 20, 20);
		cursor->setVisible(false);
		add(cursor);
	}

	void activateTargetPeptide() {
		targetPeptideGroup->setVisible(true);
	}

	void activateCurrentPeptide() {
		if (!currentPeptideGroup->isVisible()) {
			generateRibosome();
			currentPeptideGroup->setVisible(true);
		}
	}

	void activateGene() {
		geneGroup->setVisible(true);
	}

	void deactivateAll() {
		targetPeptideGroup->setVisible(false);
		currentPeptideGroup->setVisible(false);
		geneGroup->setVisible(false);
	}

	void activateAll() {
		targetPeptideGroup->setVisible(true);
		currentPeptideGroup->setVisible(true);
		geneGroup->setVisible(true);
	}

	void resetLevel() {
		initLevel();
	}

	virtual ~GameImpl() {
	}

	virtual void initGame() override {
		initLevelAndScript();
	}

	virtual void init() override {
		renderCards();
	}

	shared_ptr<AminoAcidSprite> peptideToSprite(PeptideModel &pep, shared_ptr<SpriteGroup> &group, int ax, int ay, int pos, bool full) {

		int xco = ax;
		int yco = ay;
		xco += AA_SPACING / 2;
		xco += pos * AA_STEPSIZE;

		AA aaIdx = pep.at(pos);

		auto aaSprite = make_shared<AminoAcidSprite>(xco, yco, aaIdx, full);
		group->push_back(aaSprite);

		return aaSprite;
	}

	void currentPeptideToSprite(int pos) {
		int xco = SECTION_X;
		int yco = CURRENT_PEPT_Y;
		xco += AA_SPACING / 2;
		xco += pos * AA_STEPSIZE;

		auto aa = peptideToSprite(currentPeptide, currentPeptideGroup, SECTION_X, RIBOSOME_Y, pos, false);
		world.move(aa, xco, yco, 40);
	}

	void createCheckMark(int pos) {
		// create check mark
		int xco = SECTION_X;
		int yco = CURRENT_PEPT_Y;
		xco += pos * AA_STEPSIZE;

		ALLEGRO_BITMAP *bitmap;
		if (isPositionCorrect(pos)) {
			bitmap = Engine::getResources()->getBitmap("correct");
		}
		else {
			bitmap = Engine::getResources()->getBitmap("wrong");
		}

		xco += AA_WIDTH - (al_get_bitmap_width(bitmap) / 2);
		yco -= (al_get_bitmap_height(bitmap) / 2);

		auto check = make_shared<Sprite>();
		check->setBitmap(bitmap);
		check->setxy(xco, RIBOSOME_Y);
		currentPeptideGroup->push_back(check);

		auto animator = make_shared <MoveAnimator<overshoot> >(check, xco, yco, 40);
		world.push_back(animator);
	}


	int getCurrentPeptideSize() {
		return currentPeptide.size();
	}

	void peptideToSprites(PeptideModel &pep, shared_ptr<SpriteGroup> &group, int ax, int ay, bool full) {

		group->killAll();

		int xco = ax;
		int yco = ay;

		xco += AA_SPACING / 2;

		for (size_t i = 0; i < pep.size(); ++i) {
			peptideToSprite(pep, group, ax, ay, i, full);
		}
	}

	void setUIEnabled(bool value) {
		uiEnabled = value;
	}

	void generateRibosome() {
		currentPeptideGroup->killAll();
		auto ribo = make_shared<Ribosome>(this);
		ribo->nextAnimation();
		currentPeptideGroup->push_back(ribo);
	}

	void generateGeneSprite(int pos) {
		int xco = SECTION_X + NT_STEPSIZE * pos;
		int yco = GENE_Y;

		if ((int)geneByPosition.size() <= pos) {
			geneByPosition.resize(pos + 1);
		}

		char nt = currentDNA.at(pos);

		auto ntSprite = make_shared<NucleotideSprite>(this, xco, yco, nt, pos);
		geneGroup->push_front(ntSprite); // insert below any dissolving sprites...
		geneByPosition[pos] = ntSprite;
	}

	void geneSpritesDelete(int pos) {

		if (geneByPosition[pos]) {
			geneByPosition[pos]->startDissolve();
		}

		for (size_t i = pos; i < currentDNA.size(); ++i) {
			geneByPosition[i] = geneByPosition[i+1];
			geneByPosition[i]->moveToPos(i);
		}
		geneByPosition[currentDNA.size()] = nullptr;

		generateGeneSprite(pos);
		validateGeneSprites();
	}

	void geneSpritesInsert(int pos) {

		if (geneByPosition.size() <= currentDNA.size()) {
			geneByPosition.resize(currentDNA.size());
		}

		for (int i = currentDNA.size() - 1; i > pos; i --) {
			geneByPosition[i] = geneByPosition[i-1];
			geneByPosition[i]->moveToPos(i);
		}

		generateGeneSprite(pos);
		validateGeneSprites();
	}

	void mutateGene(int pos) {
		if (geneByPosition[pos]) {
			geneByPosition[pos]->startDissolve();
		}
		generateGeneSprite(pos);
		validateGeneSprites();
	}

	void validateGeneSprites() {
		Assert(geneByPosition.size() >= currentDNA.size(), "missing a gene sprite");
		for (size_t i = 0; i < currentDNA.size(); ++i) {
			Assert (geneByPosition[i], "Missing a gene sprite");
			Assert (geneByPosition[i]->validate(i, currentDNA.at(i)), "Gene sprite does not validate");
		}
	}

	void generateGeneSprites() {

		// clear all pre-existing nucleotides.
		for (size_t i = 0; i < geneByPosition.size(); ++i) {
			if (geneByPosition[i]) { geneByPosition[i]->kill(); }
		}

		// generate fresh ones
		for (size_t i = 0; i < currentDNA.size(); ++i) {
			generateGeneSprite(i);
		}

		validateGeneSprites();
	}

	void advanceLevel() {
		currentLevel++;

		MainLoop::getMainLoop()->playSample(Engine::getResources()->getSample("sound_laser"));

		if (currentLevel >= NUM_LEVELS) {
			showVictoryMessage("You finished all levels, you won the game!",
					[=] () { pushMsg(Engine::E_QUIT); } );

		}
		else {
			showVictoryMessage("Level Complete! Press any key...",
					[=] () {
				saveCurrentLevel();
				initLevelAndScript();
			} );
		}
	}

	bool isPositionCorrect(int pos) {
		return (currentPeptide.at(pos) == targetPeptide.at(pos));
	}

	void checkWinCondition() {
		if (DNAModel::match (currentPeptide.getValue(), targetPeptide.getValue())) {

			// immediately block UI to prevent modifying the sequence any further
			setUIEnabled(false);

			// we have to wait for the Ribosome to finish. Ideally we have a signal that it's ready...
			auto t1 = Timer::build(200, [=] () {

				startScriptId(levelInfo[currentLevel].endScriptId);

			} ).get();

			add(t1);
		}
		// you used up all your cards and you haven't won
		else if (currentMutationCards.size() == 0) {
			//TODO: prevent repeat invocations
			auto t1 = Timer::build(100, [=] () {
				// display text explaining reset option.
				startScriptId(RESET_SCRIPT_ID);
			} ).get();
			add(t1);
		}

	}

	vector<ALLEGRO_SAMPLE_ID> started;

	void startLoop(const string &id)
	{
		ALLEGRO_SAMPLE_ID sampleid;
		ALLEGRO_SAMPLE *sample_data = Engine::getResources()->getSampleIfExists(id);

		if (!sample_data)
		{
			cout << "Could not find sample";
			return;
		}

		bool success = al_play_sample (sample_data, 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_LOOP, &sampleid);
		if (!success) {
			 cout << "Could not start sample";
		}

		started.push_back(sampleid);
	}

	void clearSound() {

		// stop any looped samples, leaving play-once samples untouched.
		for (vector<ALLEGRO_SAMPLE_ID>::iterator i = started.begin(); i != started.end(); ++i)
		{
			al_stop_sample(&(*i));
		}
		started.clear();
	}

	void pauseComponents(bool val) {
		drText->setAwake(val);
		patient->setAwake(val);
		world.setAwake(val);
	}

	void showVictoryMessage(const char *text, ActionFunc actionFunc) {

		startLoop("junglebeat");

		popup = Container::build().layout(Layout::LEFT_TOP_RIGHT_H, 0, 200, 0, 200).get();
		popup->add (make_shared<ClearScreen>(al_map_rgba(0, 0, 0, 128)));

		auto t1 = Text::build(WHITE, ALLEGRO_ALIGN_CENTER, string(text)).layout (Layout::LEFT_TOP_RIGHT_H, 0, 50, 0, 100).get();
		popup->add (t1);

		Resources *res = Engine::getResources();
		auto img2 = AnimComponent::build(res->getAnim("dance")).layout(Layout::CENTER_TOP_W_H, 0, 100, 64, 64).get();
		popup->add(img2);

		popupAction = [=](){
			actionFunc();
			clearSound();
		};

		add (popup);
	}

	void closePopup() {
		if (popup) {
			popup->kill();
			popup = nullptr;
		}
	}

	void startScriptId(int scriptId) {
		currentScript = scripts[scriptId].begin();
		currentScriptEnd = scripts[scriptId].end();

		mode = Mode::SCRIPT_RUNNING;
		nextScriptStep();
	}

	void initLevelAndScript() {
		Assert (currentLevel >= 0 && currentLevel < NUM_LEVELS, "currentLevel is out of range");
		LevelInfo *lev = &levelInfo[currentLevel];

		initLevel();
//		analyzePuzzle(*lev);

		deactivateAll();
		startScriptId(lev->scriptId);
	}

	shared_ptr<RichTextModel> parseText(const string &text) {
		auto result = make_shared<RichTextModel>();
		enum Modes { TEXT, TAG };
		int mode = TEXT;
		size_t start = 0;
		for (size_t i = 0; i < text.size(); ++i) {
			int c = text[i];
			switch(mode) {
				case TEXT: {
					if (c == '<') {
						if (i > start) {
							result->appendText(text.substr(start, i - start));
						}
						mode = TAG;
						start = i+1;
					}
				}
				break;
				case TAG: {
					if (c == '>') {
						string tag = text.substr(start, i - start);
						if (tag == "A" || tag == "C" || tag == "T" || tag == "G") {
							result->appendImage(Engine::getResources()->getBitmap(tag));
						}
						start = i+1;
						mode = TEXT;
					}
				}
				break;
			}
		}

		if (mode == TEXT) {
			result->appendText(text.substr(start, text.size() - start));
		}
		return result;
	}

	void setDoctorText(const string &text) {
		auto model = parseText(text);
		model->setColor(BLACK);
		model->setFont(sfont);
		drText->setTextModel(model);
		drText->startTypewriter(Text::LETTER_BY_LETTER, 1);
		drText->onAnimationComplete([=] () {

			if (currentScript == currentScriptEnd) {
				mode = Mode::MUTATION_SELECT;
			}
			else {
				mode = Mode::WAIT_FOR_KEY;
				cursor->setVisible(true);
			}

		});
	}

	void nextScriptStep() {
		bool done = false;
		while (!done)
		{

			if (currentScript == currentScriptEnd) {
				mode = Mode::MUTATION_SELECT;
				break;
			}
				// execute step

			switch (currentScript->cmd) {
				case Cmd::SAY:
					mode = Mode::SCRIPT_RUNNING;
					setDoctorText (currentScript->text);
					done = true;
					break;
				case Cmd::BIGEYES:
					patient->setState(1);
					MainLoop::getMainLoop()->playSample(Engine::getResources()->getSample("sound_scared"));
					break;
				case Cmd::NORMALEYES:
					patient->setState(0);
					break;
				case Cmd::ACTIVATE_ALL:
					activateAll();
					break;
				case Cmd::ACTIVATE_TARGET:
					activateTargetPeptide();
					break;
				case Cmd::ACTIVATE_TRANSLATION:
					activateCurrentPeptide();
					break;
				case Cmd::ACTIVATE_GENE:
					activateGene();
					break;
				case Cmd::ADVANCE_LEVEL:
					setUIEnabled(true);
					advanceLevel();
					break;
				default:
					break;
			}

			// advance pointer
			currentScript++;
		}
	}

	void initLevel() {

		targetPeptideGroup->killAll();
		currentPeptideGroup->killAll();
		geneGroup->killAll();

		Assert (currentLevel >= 0 && currentLevel < NUM_LEVELS, "currentLevel is out of range");
		LevelInfo *lev = &levelInfo[currentLevel];

		currentMutationCards = lev->mutationCards;
		initMutationCards(currentMutationCards);

		currentDNA.setValue(lev->startGene);

		Peptide pept = lev->targetPeptide;
		targetPeptide.setValue(pept);

		drText->setText("");
	}

	int getOligoSize() {
		return currentDNA.size();
	}

	void applyMutation (int pos, MutationId mutationId) {
		// first remove mutation card, before triggering level check etc...
		auto it = std::find(currentMutationCards.begin(), currentMutationCards.end(), mutationId);
		if (it != currentMutationCards.end()) {
			currentMutationCards.erase(it);
		}

		currentDNA.applyMutation(pos, mutationId);

		patient->setState(2);
		MainLoop::getMainLoop()->playSample(Engine::getResources()->getSample("sound_laser2"));
		auto t1 = Timer::build(100, [=](){ patient->setState(0); }).get();
		add(t1);
	}

	virtual void update() override {
		world.update();
		Container::update();
	}

	virtual void draw(const GraphicsContext &gc) override {
		al_clear_to_color(al_map_rgb(172,232,135));
		if (Engine::isDebug())
		{
			al_draw_text(sfont, LIGHT_BLUE, 0, 0, ALLEGRO_ALIGN_LEFT, "DEBUG ON");
		}

		world.draw(gc);
		Container::draw(gc);
	}

	void showCodonTable() {

		popup = Container::build().layout(Layout::LEFT_TOP_RIGHT_BOTTOM, 100, 100, 100, 100).get();
		popup->add (make_shared<ClearScreen>(WHITE));

		auto codonTableView = make_shared<CodonTableView>();
		codonTableView->setLayout(Layout::LEFT_TOP_RIGHT_BOTTOM, 40, 40, 40, 40);
		popup->add (codonTableView);

		popupAction = [=](){ pauseComponents(true); };
		add (popup);

		pauseComponents(false);
	}

	string keyBuffer = "";
	string levelCheatPrefix = "idclev";

	void checkCheatCode(ALLEGRO_EVENT &event) {
		if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
			keyBuffer.push_back(event.keyboard.unichar);
			if (keyBuffer.size() > 8) {
				keyBuffer = keyBuffer.substr(1);
			}

			if (keyBuffer.substr(0, 6) == levelCheatPrefix) {
				int lev = stoi(keyBuffer.substr(6, 2));
				if (lev > 0 && lev <= NUM_LEVELS) {
					currentLevel = lev - 1;
					initLevelAndScript();
				}
			}
		}
	}

	virtual void handleEvent(ALLEGRO_EVENT &event) override {
		checkCheatCode(event);

		if (popup) {
			if (event.type == ALLEGRO_EVENT_KEY_CHAR ||
				event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
					popupAction();
					closePopup();
			}
		}
		else {
			Container::handleEvent(event);
			if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
				switch (event.keyboard.keycode) {
					case ALLEGRO_KEY_F2:
						showCodonTable();
						break;
					case ALLEGRO_KEY_F1:
						resetLevel();
						break;
				}
			}
			switch (mode) {
			case Mode::POS_SELECT:
				handleEventPosSelect(event);
				break;
			case Mode::MUTATION_SELECT:
				handleEventMutationSelect(event);
				break;
			case Mode::SCRIPT_RUNNING:
				break;
			case Mode::WAIT_FOR_KEY:
				if (event.type == ALLEGRO_EVENT_KEY_CHAR ||
					event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
					cursor->setVisible(false);
					nextScriptStep();
				}
				break;

			}
		}
	}

	void moveCursorToSelectedCard(int speed) {
		if (selectedCard == cards.end()) return;
		world.move(mutationCursor,
				(*selectedCard)->origX + MUTCARD_W - 24,
				(*selectedCard)->origY + MUTCARD_H - 8,
			speed);
	}

	void moveCursorToPos(int pos, int speed) {
		int newx = (SECTION_X + NT_STEPSIZE * pos);
		world.move(mutationCursor, newx, CURSOR_Y, speed);
		mutationCursor->setPos(pos);
	}

	void handleEventPosSelect(ALLEGRO_EVENT &event) {
		// only respond to keyboard events...
		if (event.type != ALLEGRO_EVENT_KEY_CHAR) return;

		int pos = mutationCursor->getPos();
		int newpos = pos;

		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
		case ALLEGRO_KEY_A:
		case ALLEGRO_KEY_PAD_4:
		case ALLEGRO_KEY_TAB:
			newpos = max (0, pos-1);
			break;
		case ALLEGRO_KEY_RIGHT:
		case ALLEGRO_KEY_D:
		case ALLEGRO_KEY_PAD_6:
			newpos = min(pos+1, getOligoSize() - 1);
			break;
		case ALLEGRO_KEY_ESCAPE:
			// move card back to where it came from...
			world.move(*selectedCard, (*selectedCard)->origX, (*selectedCard)->origY, MOVE_SPEED_LONG);
			moveCursorToSelectedCard(MOVE_SPEED_LONG);
			mode = Mode::MUTATION_SELECT;
			break;
		case ALLEGRO_KEY_ENTER:
		case ALLEGRO_KEY_PAD_ENTER:
			if (!isUIEnabled()) {
				//TODO: error buzzer sample
			}
			else {
				applyMutation(pos, (*selectedCard)->getMutationId());

				(*selectedCard)->kill();
				selectedCard = cards.erase(selectedCard);
				if (selectedCard == cards.end()) selectedCard = cards.begin();
				if (selectedCard != cards.end()) {
					moveCursorToSelectedCard(MOVE_SPEED_LONG);
				}

				mode = Mode::MUTATION_SELECT;
			}
			break;
		}

		if (pos != newpos) {
			MainLoop::getMainLoop()->playSample(Engine::getResources()->getSample("sound_movecursor"));
			moveCursorToPos(newpos, MOVE_SPEED_SHORT);
		}
	}

	void handleEventMutationSelect(ALLEGRO_EVENT &event) {
		// only respond to keyboard events...
		if (event.type != ALLEGRO_EVENT_KEY_CHAR) return;
		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_ENTER:
		case ALLEGRO_KEY_PAD_ENTER:
			if (selectedCard != cards.end()) {
				moveCursorToPos(0, MOVE_SPEED_LONG);
				world.move(*selectedCard, SECTION_X + 0, GENE_Y + 120, MOVE_SPEED_LONG);
				MainLoop::getMainLoop()->playSample(Engine::getResources()->getSample("sound_select"));
				mode = Mode::POS_SELECT;
			}
			break;
		case ALLEGRO_KEY_LEFT:
		case ALLEGRO_KEY_UP:
		case ALLEGRO_KEY_PGUP:
		case ALLEGRO_KEY_W:
		case ALLEGRO_KEY_A:
		case ALLEGRO_KEY_PAD_4:
		case ALLEGRO_KEY_PAD_8:
			changeCardFocus(-1);
			moveCursorToSelectedCard(MOVE_SPEED_SHORT);
			break;
		case ALLEGRO_KEY_RIGHT:
		case ALLEGRO_KEY_DOWN:
		case ALLEGRO_KEY_PGDN:
		case ALLEGRO_KEY_S:
		case ALLEGRO_KEY_D:
		case ALLEGRO_KEY_PAD_6:
		case ALLEGRO_KEY_PAD_2:
		case ALLEGRO_KEY_TAB:
			changeCardFocus(+1);
			moveCursorToSelectedCard(MOVE_SPEED_SHORT);
			break;
		}
	}

	void initMutationCards(vector<MutationId> mutations) {

		// clear any remainder from previous level...
		for (auto i : cards) {
			i->kill();
		}
		cards.clear();
		mode = Mode::MUTATION_SELECT;

		int yco = mutationGroup->gety();

		int i = 0;

		for (auto mut : mutations) {
			int xco = mutationGroup->getx();
			if (i % 2 == 0) { xco += BUTTONW + 10; }

			auto mutCard = make_shared<MutationCardSprite>(mut, xco, yco);

			world.push_back(mutCard);

			if (i % 2 == 1) {
				yco += MUTCARD_H + 4;
			}

			cards.push_back(mutCard);
			i++;
		}

		selectedCard = cards.begin();

		if (mutationCursor) { mutationCursor->kill(); }
		mutationCursor = make_shared<MutationCursor>(this);
		world.push_back(mutationCursor);
		mutationCursor->setxy((*selectedCard)->origX + MUTCARD_W - 24, (*selectedCard)->origY + MUTCARD_H - 8);
	}

};

shared_ptr<Game> Game::newInstance()
{
	return make_shared<GameImpl>();
}


/*
void MutationCard::MsgLPress(const Point &p) {
	parent->createMutationCursor(mutationId);
}
*/

void NucleotideSprite::moveToPos(int _pos) {
	int xco = SECTION_X + NT_STEPSIZE * _pos;
	int yco = GENE_Y;
	pos = _pos;
	parent->world.move(shared_from_this(), xco, yco, 50);
}

void Ribosome::nextAnimation() {
	if (aaPos >= parent->getCurrentPeptideSize()) {
		kill();
		return;
	}
	else if (aaPos >= 0)
	{
		parent->currentPeptideToSprite(aaPos);

		parent->createCheckMark(aaPos);
	}

	aaPos++;

	if (aaPos >= parent->getCurrentPeptideSize()) {
		// fall down
		parent->world.move(shared_from_this(), x, 600, 50);
	}
	else {
		int xco = aaPos * AA_STEPSIZE + (AA_SPACING / 2) + SECTION_X;
		int yco = RIBOSOME_Y;

		auto animator = make_shared <MoveAnimator<sigmoid> >(shared_from_this(), xco, yco, 50);
		parent->world.push_back(animator);
	}
}
