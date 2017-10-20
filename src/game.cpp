#include <assert.h>

#include "game.h"
#include "color.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "engine.h"
#include "util.h"

using namespace std;


struct AminoAcidInfo {
	char code;
	string longCode;
	string fullName;
	vector<string> codons;
};

const int NUM_AMINO_ACIDS = 21;
AminoAcidInfo aminoAcidInfo[NUM_AMINO_ACIDS] = {
	{ 'A', "Ala", "Alanine",       vector<string>{"GCT", "GCC", "GCA", "GCG"} },
	{ 'R', "Arg", "Argenine",      vector<string>{"CGT", "CGC", "CGA", "CGG", "AGA", "AGG"} },
	{ 'N', "Asn", "Asparagine",    vector<string>{"AAT", "AAC"} },
	{ 'D', "Asp", "Aspartate",     vector<string>{"GAT", "GAC"} },
	{ 'C', "Cys", "Cysteine",      vector<string>{"TGT", "TGC"} },
	{ 'Q', "Gln", "Glutamine",     vector<string>{"CAA", "CAG"} },
	{ 'E', "Glu", "Glutamate",     vector<string>{"GAA", "GAG"} },
	{ 'G', "Gly", "Glycine",       vector<string>{"GGT", "GGC", "GGA", "GGG"} },
	{ 'H', "His", "Histidine",     vector<string>{"CAT", "CAC"} },
	{ 'I', "Ile", "Isoleucine",    vector<string>{"ATT", "ATC", "ATA"} },
	{ 'L', "Leu", "Leucine",       vector<string>{"CTT", "CTC", "CTA", "CTG", "TTA", "TTG"} },
	{ 'K', "Lys", "Lysine",        vector<string>{"AAA", "AAG"} },
	{ 'M', "Met", "Methionine",    vector<string>{"ATG"} },
	{ 'F', "Phe", "Phenylalanine", vector<string>{"TTT", "TTC"} },
	{ 'P', "Pro", "Proline",       vector<string>{"CCT", "CCC", "CCA", "CCG"} },
	{ 'S', "Ser", "Serine",        vector<string>{"TCT", "TCC", "TCA", "TCG", "AGT", "AGC"} },
	{ 'T', "Thr", "Threonine",     vector<string>{"ACT", "ACC", "ACA", "ACG"} },
	{ 'W', "Trp", "Tryptophan",    vector<string>{"TGG"} },
	{ 'Y', "Tyr", "Tyrosine",      vector<string>{"TAT", "TAC"} },
	{ 'V', "Val", "Valine",        vector<string>{"GTT", "GTC", "GTA", "GTG"} },
	{ '*', "***", "Stop",          vector<string>{"TAA", "TAG", "TGA"} },
};

enum class MutationId {
	TRANSVERSION, TRANSITION, COMPLELENT, REVERSE_COMPLEMENT, INSERTION_A, INSERTION_C, INERTION_T, INSERTION_G, DELETION,
};

struct MutationInfo {
	int cardId;
};

const int NUM_NUCLEOTIDES = 4;
struct NucleotideInfo {
	char code;
	string name;
};

struct LevelInfo {
	vector<int> cards;
};


int getNucleotideIndex(char c) {
	switch(c) {
	case 'T': return 0;
	case 'C': return 1;
	case 'A': return 2;
	case 'G': return 3;
	default: Assert (false, "Not a valid nucleotide character");
		return 0;
	break;
	}

}

int getCodonIndex(int i, int j, int k) {
	Assert (i >= 0 && i < NUM_NUCLEOTIDES, "Invalid nucleotide");
	Assert (j >= 0 && j < NUM_NUCLEOTIDES, "Invalid nucleotide");
	Assert (k >= 0 && k < NUM_NUCLEOTIDES, "Invalid nucleotide");

	return i + j * NUM_NUCLEOTIDES + k * NUM_NUCLEOTIDES * NUM_NUCLEOTIDES;
};

int codonIndexFromString(const string &val) {
	Assert (val.size() == 3, "Not a valid codon string");

	return getCodonIndex(
		getNucleotideIndex(val.at(0)),
		getNucleotideIndex(val.at(1)),
		getNucleotideIndex(val.at(2))
	);
}

class CodonTable {
private:
	vector<const AminoAcidInfo *> codonTable;
public:
	CodonTable() {
		codonTable.resize(64);
		for (int i = 0; i < NUM_AMINO_ACIDS; ++i) {

			for (auto codon : aminoAcidInfo[i].codons) {
				int codonIdx = codonIndexFromString(codon);
				codonTable[codonIdx] = &aminoAcidInfo[i];
			}
		}
	}

	const AminoAcidInfo *getCodon(int i, int j, int k) {
		int idx = getCodonIndex(i, j, k);
		return codonTable[idx];
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
					const AminoAcidInfo *aai = codonTable.getCodon(i, j, k);
					if (aai) {
						al_draw_text(sfont, GREEN, xco, yco, ALLEGRO_ALIGN_LEFT, aai->longCode.c_str());
					}

				}
			}
		}
	};

};

class GameImpl : public Game {
private:
	int currentLevel;
	shared_ptr<CodonTableView> codonTableView;
public:
	GameImpl() : currentLevel(0) {
		codonTableView = make_shared<CodonTableView>();
		codonTableView->setLayout(Layout::LEFT_TOP_RIGHT_BOTTOM, 40, 40, 40, 40);
		add (codonTableView);
	}

	virtual ~GameImpl() {

	}

	virtual void initGame() override {
	}

	void initLevel() {
	}

	virtual void update() override {
	}

	virtual void draw(const GraphicsContext &gc) override {
		al_clear_to_color(BLACK);
		if (Engine::isDebug())
		{
			al_draw_text(Engine::getFont(), LIGHT_BLUE, 0, 0, ALLEGRO_ALIGN_LEFT, "DEBUG ON");
		}

		al_draw_text(Engine::getFont(), LIGHT_BLUE, getw() / 2, 100, ALLEGRO_ALIGN_CENTER, "Hello World");

		Container::draw(gc);
	}

	virtual void handleEvent(ALLEGRO_EVENT &event) override {
	}

	virtual void init() override {
	}

};

shared_ptr<Game> Game::newInstance()
{
	return make_shared<GameImpl>();
}


