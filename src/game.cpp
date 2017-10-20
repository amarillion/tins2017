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
public:
	NucleotideSprite(double x, double y, char code) : code(code) {
		this->x = x;
		this->y = y;
		w = 32;
		h = 64;
		info = &nucleotideInfo[getNucleotideIndex(code)];
	}

	virtual void draw(const GraphicsContext &gc) {
		double x1 = x + gc.xofst;
		double y1 = y + gc.yofst;

		al_draw_filled_rectangle(x1, y1, x1 + w, y1 + h, al_map_rgb_f(0.5, 0, 0));
		al_draw_rectangle(x1, y1, x1 + w, y1 + h, RED, 1.0);

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
		w = 96;
		h = 64;
		Assert (aaIdx >= 0 && aaIdx < NUM_AMINO_ACIDS, "Invalid amino acid index");
		info = &aminoAcidInfo[aaIdx];
	}

	virtual void draw(const GraphicsContext &gc) {
		double x1 = x + gc.xofst;
		double y1 = y + gc.yofst;

		al_draw_filled_rectangle(x1, y1, x1 + w, y1 + h, al_map_rgb_f(0.5, 0, 0));
		al_draw_rectangle(x1, y1, x1 + w, y1 + h, RED, 1.0);

		draw_shaded_textf(font, WHITE, GREY, x1 + 15, y1 + 5, ALLEGRO_ALIGN_LEFT, info->threeLetterCode.c_str());
	};

};

class PeptideModel {

private:
	vector<int> pept;
public:

	PeptideModel() : pept() {
	}

	PeptideModel (vector<int> pept) : pept(pept) {

	}

	size_t size() {
		return pept.size();
	}

	int at(int idx) {
		return pept[idx];
	}

};

class DNAModel {
private:
	string data;
public:
	DNAModel() : data() {
	}

	DNAModel(string init) {
		data = init;
	};

	size_t size() {
		return data.size();
	}

	char at(int idx) {
		return data.at(idx);
	}

	PeptideModel translate() {

		vector<int> pept;

		for (size_t i = 0; i < data.size(); i += 3) {

			int aa = codonTable.getCodon(
					getNucleotideIndex(at(i)),
					getNucleotideIndex(at(i+1)),
					getNucleotideIndex(at(i+2))
			);
			pept.push_back(aa);
		}

		return PeptideModel(pept);
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
		Assert (pos >= 0 && pos < data.size(), "pos is out of range");
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
		setPos(0);
	}

	virtual void draw(const GraphicsContext &gc) override {
		double x1 = x + gc.xofst;
		double y1 = y + gc.yofst;

		al_draw_filled_rectangle(x1, y1, x1 + w, y1 + h, al_map_rgb_f(0, 0.5, 0));
		al_draw_rectangle(x1, y1, x1 + w, y1 + h, GREEN, 1.0);

		draw_shaded_textf(font, WHITE, GREY, x1 + 15, y1 + 5, ALLEGRO_ALIGN_LEFT, "cursor");
	}

	void setPos(int newpos) {
		if (pos != newpos) {
			pos = newpos;
			setx( 64 * pos);
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

class MutationMenu : public Panel {
private:
public:
	MutationMenu() {
//		auto btn = Button::build(0, "Reverse Complement").layout (Layout::LEFT TOP_W_H, 10, 10, 80, 20).build();
//		add (btn);
	}
};

class GameImpl : public Game {
private:
	int currentLevel;
	shared_ptr<CodonTableView> codonTableView;

	list<shared_ptr<Sprite>> sprites;

	DNAModel currentDNA;
	PeptideModel targetPeptide;
	PeptideModel currentPeptide;

	shared_ptr<MutationCursor> mutationCursor;
	shared_ptr<MutationMenu> menu;
public:

	GameImpl() : currentLevel(0), sprites() {
		codonTableView = make_shared<CodonTableView>();
		codonTableView->setLayout(Layout::LEFT_TOP_RIGHT_BOTTOM, 40, 40, 40, 40);
		codonTableView->setVisible(false); // start hidden
		add (codonTableView);

		menu = make_shared<MutationMenu>();
		menu->setLayout(Layout::RIGHT_BOTTOM_W_H, 10, 10, 200, 200);
		add(menu);

		mutationCursor = make_shared<MutationCursor>(currentDNA, MutationId::COMPLEMENT);
		mutationCursor->setxy(100, 400);
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

	void peptideToSprites(PeptideModel &pep, int ax, int ay) {

		int xco = ax;
		int yco = ay;

		for (size_t i = 0; i < pep.size(); ++i) {
			int aaIdx = pep.at(i);

			auto aaSprite = make_shared<AminoAcidSprite>(xco, yco, aaIdx);
			sprites.push_back(aaSprite);

			xco += 160;
		}

	}

	void generateCureSprites() {

		LevelInfo *lev = &levelInfo[currentLevel];

		peptideToSprites(targetPeptide, 10, 100);
		peptideToSprites(currentPeptide, 10, 200);
	}

	void generateGeneSprites() {

		int xco = 10;
		int yco = 300;

		for (size_t i = 0; i < currentDNA.size(); ++i) {
			char nt = currentDNA.at(i);

			auto ntSprite = make_shared<NucleotideSprite>(xco, yco, nt);
			sprites.push_back(ntSprite);
			xco += 48;
		}

	}

	void initLevel() {

		LevelInfo *lev = &levelInfo[currentLevel];

		currentDNA = DNAModel(lev->startGene);

		currentPeptide = currentDNA.translate();

		vector<int> pept;
		for (auto aa : lev->targetPeptide) {
			int aaIdx = codonTable.getIndexByThreeLetterCode(aa);
			pept.push_back(aaIdx);
		}
		targetPeptide = PeptideModel(pept);

		generateCureSprites();
		generateGeneSprites();
	}

	virtual void update() override {
		for (auto i : sprites)
		{
			if (i->isAlive()) i->update();
		}

		sprites.remove_if ( [](shared_ptr<Sprite> i) { return !(i->isAlive()); });
	}

	virtual void draw(const GraphicsContext &gc) override {
		al_clear_to_color(BLACK);
		if (Engine::isDebug())
		{
			al_draw_text(Engine::getFont(), LIGHT_BLUE, 0, 0, ALLEGRO_ALIGN_LEFT, "DEBUG ON");
		}

		Container::draw(gc);

		for (auto i : sprites)
		{
			if (i->isAlive() && i->isVisible()) i->draw(gc);
		}
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


