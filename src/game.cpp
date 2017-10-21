#include <assert.h>

#include "game.h"
#include "color.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "engine.h"
#include "util.h"
#include "textstyle.h"
#include "button.h"
#include "panel.h"
#include "button.h"

#include "data.h"

using namespace std;

enum class AminoAcid {
	A, R, N, D, C, Q, E, G, H, I, L, K, M, F, P, S, T, W, Y, V, STOP
};

struct AminoAcidInfo {
	char code;
	string threeLetterCode;
	string fullName;
	vector<string> codons;
};

const int NUM_AMINO_ACIDS = 21;
AminoAcidInfo aminoAcidInfo[NUM_AMINO_ACIDS] = {
	{ 'A', "Ala", "Alanine",       {"GCT", "GCC", "GCA", "GCG"} },
	{ 'R', "Arg", "Argenine",      {"CGT", "CGC", "CGA", "CGG", "AGA", "AGG"} },
	{ 'N', "Asn", "Asparagine",    {"AAT", "AAC"} },
	{ 'D', "Asp", "Aspartate",     {"GAT", "GAC"} },
	{ 'C', "Cys", "Cysteine",      {"TGT", "TGC"} },
	{ 'Q', "Gln", "Glutamine",     {"CAA", "CAG"} },
	{ 'E', "Glu", "Glutamate",     {"GAA", "GAG"} },
	{ 'G', "Gly", "Glycine",       {"GGT", "GGC", "GGA", "GGG"} },
	{ 'H', "His", "Histidine",     {"CAT", "CAC"} },
	{ 'I', "Ile", "Isoleucine",    {"ATT", "ATC", "ATA"} },
	{ 'L', "Leu", "Leucine",       {"CTT", "CTC", "CTA", "CTG", "TTA", "TTG"} },
	{ 'K', "Lys", "Lysine",        {"AAA", "AAG"} },
	{ 'M', "Met", "Methionine",    {"ATG"} },
	{ 'F', "Phe", "Phenylalanine", {"TTT", "TTC"} },
	{ 'P', "Pro", "Proline",       {"CCT", "CCC", "CCA", "CCG"} },
	{ 'S', "Ser", "Serine",        {"TCT", "TCC", "TCA", "TCG", "AGT", "AGC"} },
	{ 'T', "Thr", "Threonine",     {"ACT", "ACC", "ACA", "ACG"} },
	{ 'W', "Trp", "Tryptophan",    {"TGG"} },
	{ 'Y', "Tyr", "Tyrosine",      {"TAT", "TAC"} },
	{ 'V', "Val", "Valine",        {"GTT", "GTC", "GTA", "GTG"} },
	{ '*', "***", "Stop",          {"TAA", "TAG", "TGA"} },
};

enum class MutationId {
	TRANSVERSION, TRANSITION, COMPLEMENT, REVERSE_COMPLEMENT, INSERTION_A, INSERTION_C, INSERTION_T, INSERTION_G, DELETION,
};

struct MutationInfo {
	int cardId;
};

const int NUM_NUCLEOTIDES = 4;
struct NucleotideInfo {
	char code;
	string name;
};

enum class Nucleotide { T, C, A, G };

NucleotideInfo nucleotideInfo[NUM_NUCLEOTIDES] = {
	{ 'T', "Tyrosine" },
	{ 'C', "Cytosine" },
	{ 'A', "Adenine" },
	{ 'G', "Guanosine" }
};

struct LevelInfo {
	vector<string> targetPeptide;
	string startGene;

	// cards available in this level
	vector<MutationId> cards;
};

const int NUM_LEVELS = 1;
LevelInfo levelInfo[NUM_LEVELS] = {
	{ { "Trp", "Val", "Thr" }, "TGAGTTACC", { MutationId::TRANSVERSION } }
};

int getNucleotideIndex(char c) {
	switch(c) {
	//TODO: use reverse lookup of NucleotideInfo instead of hard-coding order...
	case 'T': return 0;
	case 'C': return 1;
	case 'A': return 2;
	case 'G': return 3;
	default: Assert (false, "Not a valid nucleotide character");
		return 0;
	break;
	}
}

ALLEGRO_COLOR getNucleotideColor(int idx, float shade) {
	Assert (shade >= 0 && shade <= 1.0, "shade is not in range");
	Assert (idx >= 0 && idx < NUM_NUCLEOTIDES, "idx is not in range");

	switch (idx) {
	case 0: return al_map_rgb_f(shade * 1.0, 0.0, 0.0); // T -> RED
	case 1: return al_map_rgb_f(0.0, 0.0, shade * 1.0); // C -> BLUE
	case 2: return al_map_rgb_f(0.0, 1.0 * shade, 0.0); // A -> GREEN
	case 3: return al_map_rgb_f(0.5 * shade, 0.5 * shade, 0.5 * shade); // G -> BLACK(or grey)
	default: return BLACK;
	}
}

class CodonTable {
private:
	vector<int> codonTable;
	map<string, int> aaIndexByThreeLetterCode;
public:
	CodonTable() {

		codonTable.resize(64);
		for (int i = 0; i < NUM_AMINO_ACIDS; ++i) {

			auto aaInfo = &aminoAcidInfo[i];
			for (auto codon : aaInfo->codons) {
				int codonIdx = codonIndexFromString(codon);
				codonTable[codonIdx] = i;
			}

			aaIndexByThreeLetterCode[aaInfo->threeLetterCode] = i;
		}

	}

	int getCodonIndex(int i, int j, int k) {
		Assert (i >= 0 && i < NUM_NUCLEOTIDES, "Invalid nucleotide");
		Assert (j >= 0 && j < NUM_NUCLEOTIDES, "Invalid nucleotide");
		Assert (k >= 0 && k < NUM_NUCLEOTIDES, "Invalid nucleotide");

		return i + j * NUM_NUCLEOTIDES + k * NUM_NUCLEOTIDES * NUM_NUCLEOTIDES;
	};

	/** Converts "ATG" to the index of the corresponding amino acid */
	int codonIndexFromString(const string &val) {
		Assert (val.size() == 3, "Not a valid codon string");

		return getCodonIndex(
			getNucleotideIndex(val.at(0)),
			getNucleotideIndex(val.at(1)),
			getNucleotideIndex(val.at(2))
		);
	}

	int getCodon(int i, int j, int k) {
		int idx = getCodonIndex(i, j, k);
		return codonTable[idx];
	}

	int getIndexByThreeLetterCode(const string &threeLetterCode) {
		Assert (threeLetterCode.size() == 3, "Three letter code must have three letters");
		Assert (aaIndexByThreeLetterCode.find(threeLetterCode) != aaIndexByThreeLetterCode.end(), "Unknown three-letter code");
		return aaIndexByThreeLetterCode[threeLetterCode];
	}
};

CodonTable codonTable;

class CodonTableView : public IComponent {

	virtual void draw(const GraphicsContext &gc) override {

		int x1 = getx() + gc.xofst;
		int y1 = gety() + gc.yofst;

		al_draw_rectangle(x1 + 1, y1 + 1, getw() - 1, geth() - 1, WHITE, 1.0);

		const int COL_WIDTH = 80;
		const int OUTER_ROW_HEIGHT = 64;
		const int INNER_ROW_HEIGHT = 16;

		for (int i = 0; i < NUM_NUCLEOTIDES; ++i) {
			for (int j = 0; j < NUM_NUCLEOTIDES; ++j) {
				for (int k = 0; k < NUM_NUCLEOTIDES; ++k) {

					// cell position
					int xco = x1 + j * COL_WIDTH + getx();
					int yco = y1 + i * OUTER_ROW_HEIGHT + k * INNER_ROW_HEIGHT;
					int aaIdx = codonTable.getCodon(i, j, k);
					const AminoAcidInfo *aai = &aminoAcidInfo[aaIdx];
					if (aai) {
						al_draw_text(sfont, GREEN, xco, yco, ALLEGRO_ALIGN_LEFT, aai->threeLetterCode.c_str());
					}

				}
			}
		}
	};

};

// generic Layout variables
const int AA_WIDTH = 96;
const int AA_SPACING = 16; // between AA cards
const int AA_HEIGHT = 64;
const int AA_STEPSIZE = AA_WIDTH + AA_SPACING;

const int AA_PADDING = 8; // padding on inside of AA card.

const int NT_WIDTH = 30;
const int NT_HEIGHT = 2 * NT_WIDTH;
const int NT_SPACING = 8;
const int NT_STEPSIZE = NT_WIDTH + NT_SPACING;


const int BUTTONW = 120;
const int BUTTONH = 16;

ALLEGRO_FONT *font;

class Sprite
{
protected:
	double x; // cell-x
	double y;

	int w;
	int h;

	bool alive;
	bool visible;

public:
	bool isAlive() { return alive; }
	void kill() { alive = false; /* scheduled to be removed at next update from any list that is updated */ }
	bool isVisible() { return visible; }
	virtual void draw(const GraphicsContext &gc) {};
	virtual void update() {};

	Sprite() : x(0), y(0), w(16), h(16), alive(true), visible(true) {}
	virtual ~Sprite() {}
};

class NucleotideSprite : public Sprite {
private:

	NucleotideInfo *info;
	char code;
	int idx;
public:
	NucleotideSprite(double x, double y, char code) : code(code) {
		this->x = x;
		this->y = y;
		w = NT_WIDTH;
		h = NT_HEIGHT;
		idx = getNucleotideIndex(code);
		info = &nucleotideInfo[idx];
	}

	virtual void draw(const GraphicsContext &gc) {
		double x1 = x + gc.xofst;
		double y1 = y + gc.yofst;

		ALLEGRO_COLOR mainColor = getNucleotideColor(idx, 0.5);
		ALLEGRO_COLOR shadeColor = getNucleotideColor(idx, 1.0);

		al_draw_filled_rectangle(x1, y1, x1 + w, y1 + h, shadeColor);
		al_draw_rectangle(x1, y1, x1 + w, y1 + h, mainColor, 1.0);

		draw_shaded_textf(font, WHITE, GREY, x1 + 5, y1 + 5, ALLEGRO_ALIGN_LEFT, "%c", info->code);
	};
};


class AminoAcidSprite : public Sprite {
private:
	AminoAcidInfo *info;
	int aaIdx;
public:
	AminoAcidSprite(double x, double y, int aaIdx) : aaIdx(aaIdx) {

		this->x = x;
		this->y = y;
		w = AA_WIDTH;
		h = AA_HEIGHT;
		Assert (aaIdx >= 0 && aaIdx < NUM_AMINO_ACIDS, "Invalid amino acid index");
		info = &aminoAcidInfo[aaIdx];
	}

	virtual void draw(const GraphicsContext &gc) {
		double x1 = x + gc.xofst;
		double y1 = y + gc.yofst;

		al_draw_filled_rectangle(x1, y1, x1 + w, y1 + h, al_map_rgb_f(0.5, 0, 0));
		al_draw_rectangle(x1, y1, x1 + w, y1 + h, RED, 1.0);

		draw_shaded_textf(font, WHITE, GREY, x1 + AA_PADDING, y1 + AA_PADDING, ALLEGRO_ALIGN_LEFT, info->threeLetterCode.c_str());

		// draw the DNA target sequences...

		int yco = y1 + 20;
		const int INTERNAL_SPACING = 4;
		const int INTERNAL_WIDTH = (AA_WIDTH - (2 * AA_PADDING) - (2 * INTERNAL_SPACING)) / 3;
		const int INTERNAL_STEPSIZE = INTERNAL_WIDTH + INTERNAL_SPACING;
		const int INTERNAL_HEIGHT = 5;
		const int INTERNAL_VERTICAL_STEPSIZE = INTERNAL_HEIGHT + 2;

		for (auto codon : info->codons) {
			for (int c = 0; c < 3; ++c) {

				int xco = x1 + c * INTERNAL_STEPSIZE + AA_PADDING;

				char nt = codon.at(c);
				int ntIdx = getNucleotideIndex(nt);
				ALLEGRO_COLOR mainCol = getNucleotideColor(ntIdx, 1.0);
				ALLEGRO_COLOR shadeCol = getNucleotideColor(ntIdx, 0.5);

				al_draw_filled_rectangle(xco, yco, xco + INTERNAL_WIDTH, yco + INTERNAL_HEIGHT, shadeCol);
				al_draw_rectangle(xco, yco, xco + INTERNAL_WIDTH, yco + INTERNAL_HEIGHT, mainCol, 1.0);
			}
			yco += INTERNAL_VERTICAL_STEPSIZE;
		}
	};

};

/** grouping of sprites
 *
 * Note that sprites may always be members of multiple groups.
 *
 * SpriteGroups may be used for
 * - drawing
 * - updating
 * - z-ranking
 * collections of sprites
 *
 * But also for laying them out together, building a hierarchy of sprites,
 * or simply remembering which group of sprites belong together
 */
class SpriteGroup : public Sprite {
private:
	list<shared_ptr<Sprite>> sprites;

public:

	void push_back(const shared_ptr<Sprite> &val) {
		sprites.push_back(val);
	}

	void killAll() {
		for (auto sp : sprites) {
			sp->kill();
		}
		sprites.clear();
	}

	virtual void update() {
		for (auto i : sprites)
		{
			if (i->isAlive()) i->update();
		}

		sprites.remove_if ( [](shared_ptr<Sprite> i) { return !(i->isAlive()); });
	}

	virtual void draw (const GraphicsContext &gc) {
		for (auto i : sprites)
		{
			if (i->isAlive() && i->isVisible()) i->draw(gc);
		}
	}

};

using Peptide = vector<int>;
using OligoNt = string;

enum { EVT_PEPT_CHANGED = 1, EVT_OLIGO_CHANGED };

class PeptideModel : public DataWrapper {

private:
	Peptide data;
public:

	PeptideModel() : data() {
	}

	void setValue(Peptide val) {
		if (data != val) {
			data = val;
			FireEvent(EVT_PEPT_CHANGED);
		}
	}

	size_t size() {
		return data.size();
	}

	int at(int idx) {
		return data[idx];
	}

	Peptide getValue() {
		return data; // should be copy
	}
};

class DNAModel : public DataWrapper {
private:
	OligoNt data;
public:
	DNAModel() : data() {
	}

	void setValue(OligoNt val) {
		if (data != val) {
			data = val;
			FireEvent(EVT_OLIGO_CHANGED);
		}
	}

	size_t size() {
		return data.size();
	}

	char at(int idx) {
		return data.at(idx);
	}

	Peptide translate() {

		Peptide pept;

		for (size_t i = 0; i < data.size(); i += 3) {

			int aa = codonTable.getCodon(
					getNucleotideIndex(at(i)),
					getNucleotideIndex(at(i+1)),
					getNucleotideIndex(at(i+2))
			);
			pept.push_back(aa);
		}

		return pept;
	}

	char getComplement(char nt) {
		switch (nt) {
			case 'A': return 'T';
			case 'T': return 'A';
			case 'G': return 'C';
			case 'C': return 'G';
			default: Assert (false, "Invalid nucleotide");
			return ' ';
		}
	}

	char getTransversion(char nt) {
		switch (nt) {
			case 'G': return 'T';
			case 'T': return 'G';
			case 'A': return 'C';
			case 'C': return 'A';
			default: Assert (false, "Invalid nucleotide");
			return ' ';
		}
	}

	char getTransition(char nt) {
		switch (nt) {
			case 'G': return 'A';
			case 'A': return 'G';
			case 'T': return 'C';
			case 'C': return 'T';
			default: Assert (false, "Invalid nucleotide");
			return ' ';
		}
	}

	void applyMutation (int pos, MutationId mutation) {
		Assert (pos >= 0 && pos < (int)data.size(), "pos is out of range");
		OligoNt oldData = data;
		switch (mutation) {
		case MutationId::COMPLEMENT:
			data[pos] = getComplement(data[pos]);
			break;
		case MutationId::TRANSVERSION:
			data[pos] = getTransversion(data[pos]);
			break;
		case MutationId::TRANSITION:
			data[pos] = getTransition(data[pos]);
			break;
		case MutationId::DELETION:
			data.erase(pos, 1);
			break;
		case MutationId::REVERSE_COMPLEMENT:
			reverseComplement();
			break;
		case MutationId::INSERTION_A:
			data.insert(pos, "A");
			break;
		case MutationId::INSERTION_C:
			data.insert(pos, "C");
			break;
		case MutationId::INSERTION_G:
			data.insert(pos, "G");
			break;
		case MutationId::INSERTION_T:
			data.insert(pos, "T");
			break;
		default:
			Assert (false, "Invalid mutation id");
			break;
		}

		if (oldData != data) {
			FireEvent(EVT_OLIGO_CHANGED);
		}
	}

	/** in-place modification. Turn this DNA sequence into its reverse complement */
	void reverseComplement() {
		string newData;
		for (char c : data) {
			newData += getComplement(c);
		}

		data = newData;
	}
};

class MutationCursor : public IComponent {
	DNAModel &model;
	int pos;
	MutationId mutation;
public:
	MutationCursor(DNAModel &model, MutationId mutation) : model(model), pos(0), mutation(mutation) {
		w = 32;
		h = 32;
		setLocation(0, 0, w, h);
		setPos(0);
	}

	virtual void draw(const GraphicsContext &gc) override {
		double x1 = getx() + gc.xofst;
		double y1 = gety() + gc.yofst;

		al_draw_filled_rectangle(x1, y1, x1 + w, y1 + h, al_map_rgb_f(0, 0.5, 0));
		al_draw_rectangle(x1, y1, x1 + w, y1 + h, GREEN, 1.0);

		draw_shaded_textf(font, WHITE, GREY, x1 + 15, y1 + 5, ALLEGRO_ALIGN_LEFT, "cursor");
	}

	void setPos(int newpos) {
		if (pos != newpos) {
			pos = newpos;
			setx(NT_STEPSIZE * pos);
		}
	}

	virtual void handleEvent(ALLEGRO_EVENT &event) override {
		int newpos = pos;
		if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_LEFT:
				newpos = max (0, pos-1);
				break;
			case ALLEGRO_KEY_RIGHT:
				newpos = min(pos+1, (int)model.size());
				break;
			case ALLEGRO_KEY_ENTER:
				model.applyMutation(pos, mutation);
				break;
			}
		}

		setPos(newpos);
	};
};

class MutationMenu : public Container {
private:
public:
	MutationMenu() {

		add (Button::build(0, "Rev. Comp.").layout(Layout::LEFT_TOP_W_H, 10, 10, BUTTONW, BUTTONH).get());
		add (Button::build(0, "Transition").layout(Layout::LEFT_TOP_W_H, 10, 30, BUTTONW, BUTTONH).get());
		add (Button::build(0, "Transversion").layout(Layout::LEFT_TOP_W_H, 10, 50, BUTTONW, BUTTONH).get());
		add (Button::build(0, "Complement").layout(Layout::LEFT_TOP_W_H, 10, 70, BUTTONW, BUTTONH).get());
		add (Button::build(0, "Insert A").layout(Layout::LEFT_TOP_W_H, 100, 10, BUTTONW, BUTTONH).get());
		add (Button::build(0, "Insert T").layout(Layout::LEFT_TOP_W_H, 100, 30, BUTTONW, BUTTONH).get());
		add (Button::build(0, "Insert C").layout(Layout::LEFT_TOP_W_H, 100, 50, BUTTONW, BUTTONH).get());
		add (Button::build(0, "Insert G").layout(Layout::LEFT_TOP_W_H, 100, 70, BUTTONW, BUTTONH).get());
		add (Button::build(0, "Deletion").layout(Layout::LEFT_TOP_W_H, 100, 90, BUTTONW, BUTTONH).get());
	}};

class GameImpl : public Game {
private:
	int currentLevel;
	shared_ptr<CodonTableView> codonTableView;

	SpriteGroup world;
	SpriteGroup geneGroup;
	SpriteGroup targetPeptideGroup;
	SpriteGroup currentPeptideGroup;

	DNAModel currentDNA;
	PeptideModel targetPeptide;
	PeptideModel currentPeptide;

	shared_ptr<MutationCursor> mutationCursor;
	shared_ptr<MutationMenu> menu;


public:

	GameImpl() : currentLevel(0), world() {
		codonTableView = make_shared<CodonTableView>();
		codonTableView->setLayout(Layout::LEFT_TOP_RIGHT_BOTTOM, 40, 40, 40, 40);
		codonTableView->setVisible(false); // start hidden
		add (codonTableView);

		menu = make_shared<MutationMenu>();
		menu->setLayout(Layout::RIGHT_BOTTOM_W_H, 10, 10, 200, 200);
		add(menu);

		mutationCursor = make_shared<MutationCursor>(currentDNA, MutationId::COMPLEMENT);
		mutationCursor->sety(400);
		add(mutationCursor);
	}

	virtual ~GameImpl() {
	}

	virtual void initGame() override {
		initLevel();
	}

	virtual void init() override {
		font = al_create_builtin_font(); // TODO fragile initialization of global... Wrap font in a Singleton...
		initGame();
	}

	void peptideToSprites(PeptideModel &pep, SpriteGroup &group, int ax, int ay) {

		group.killAll();

		int xco = ax;
		int yco = ay;

		for (size_t i = 0; i < pep.size(); ++i) {
			int aaIdx = pep.at(i);

			auto aaSprite = make_shared<AminoAcidSprite>(xco, yco, aaIdx);
			world.push_back(aaSprite);
			group.push_back(aaSprite);

			xco += AA_STEPSIZE;
		}

	}

	void generateGeneSprites(DNAModel &oligo) {

		int xco = 10;
		int yco = 300;

		geneGroup.killAll();

		for (size_t i = 0; i < oligo.size(); ++i) {
			char nt = oligo.at(i);

			auto ntSprite = make_shared<NucleotideSprite>(xco, yco, nt);
			world.push_back(ntSprite);
			geneGroup.push_back(ntSprite);
			xco += NT_STEPSIZE;
		}

	}

	void checkWinCondition() {
		if (currentPeptide.getValue() == targetPeptide.getValue()) {
			cout << "You've completed the level!" << endl;
		}
	}

	void initLevel() {

		LevelInfo *lev = &levelInfo[currentLevel];

		currentDNA.AddListener( [=] (int code) {
			cout << "Current DNA updated" << endl;
			currentPeptide.setValue(currentDNA.translate());
			generateGeneSprites(currentDNA);
		});

		currentPeptide.AddListener( [=] (int code) {
			cout << "Current peptide updated" << endl;
			peptideToSprites(currentPeptide, currentPeptideGroup, 10, 200);
			checkWinCondition();
		});

		targetPeptide.AddListener( [=] (int code) {
			cout << "Target peptide updated" << endl;
			peptideToSprites(targetPeptide, targetPeptideGroup, 10, 100);
		});


		currentDNA.setValue(lev->startGene);

		Peptide pept;
		for (auto aa : lev->targetPeptide) {
			int aaIdx = codonTable.getIndexByThreeLetterCode(aa);
			pept.push_back(aaIdx);
		}
		targetPeptide.setValue(pept);

	}

	virtual void update() override {
		world.update();
	}

	virtual void draw(const GraphicsContext &gc) override {
		al_clear_to_color(WHITE);
		if (Engine::isDebug())
		{
			al_draw_text(Engine::getFont(), LIGHT_BLUE, 0, 0, ALLEGRO_ALIGN_LEFT, "DEBUG ON");
		}

		Container::draw(gc);

		world.draw(gc);
	}

	virtual void handleEvent(ALLEGRO_EVENT &event) override {
		if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
			switch (event.keyboard.keycode) {
				case ALLEGRO_KEY_F2:
					codonTableView->setVisible(!codonTableView->isVisible());
					break;
			}
		}

		if (mutationCursor) mutationCursor->handleEvent(event);
	}

};

shared_ptr<Game> Game::newInstance()
{
	return make_shared<GameImpl>();
}


